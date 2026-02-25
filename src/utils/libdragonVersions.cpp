/**
* @copyright 2026 - Max Bebök
* @license MIT
*/
#include "libdragonVersions.h"
#include "proc.h"
#include "json.hpp"
#include <thread>

namespace
{
  // Parse ISO date "2025-02-24T15:30:00Z" to "Feb 24"
  std::string formatDate(const std::string &isoDate)
  {
    if(isoDate.size() < 10) return isoDate;

    constexpr const char* MONTHS[] = {
      "Jan","Feb","Mar","Apr","May","Jun",
      "Jul","Aug","Sep","Oct","Nov","Dec"
    };

    try {
      int month = std::stoi(isoDate.substr(5, 2));
      int day = std::stoi(isoDate.substr(8, 2));
      if(month >= 1 && month <= 12) {
        return std::string(MONTHS[month-1]) + " " + std::to_string(day);
      }
    } catch(...) {}

    return isoDate.substr(0, 10);
  }

  // Extract first line from a potentially multi-line commit message
  std::string firstLine(const std::string &msg)
  {
    auto pos = msg.find('\n');
    auto line = (pos != std::string::npos) ? msg.substr(0, pos) : msg;
    if(line.size() > 60) {
      line = line.substr(0, 57) + "...";
    }
    return line;
  }
}

void Utils::LibdragonVersions::fetchCommits()
{
  auto current = state.load();
  if(current == FetchState::FETCHING) return;
  state.store(FetchState::FETCHING);

  std::thread([this]() {
    std::string cmd = "curl -s -m 10 "
      "\"https://api.github.com/repos/DragonMinded/libdragon/commits"
      "?sha=preview&per_page=30\"";

    auto result = Utils::Proc::runSync(cmd);
    if(result.empty()) {
      std::lock_guard lock(mtx);
      errorMsg = "Could not fetch versions. Check your internet connection.";
      state.store(FetchState::ERROR);
      return;
    }

    try {
      auto json = nlohmann::json::parse(result);
      if(!json.is_array()) {
        std::lock_guard lock(mtx);
        errorMsg = "Unexpected response from GitHub API.";
        state.store(FetchState::ERROR);
        return;
      }

      std::vector<LibdragonCommit> parsed;
      for(auto &entry : json) {
        parsed.push_back({
          entry["sha"].get<std::string>(),
          firstLine(entry["commit"]["message"].get<std::string>()),
          formatDate(entry["commit"]["author"]["date"].get<std::string>())
        });
      }

      std::lock_guard lock(mtx);
      commits = std::move(parsed);
      state.store(FetchState::DONE);
    } catch(const std::exception &e) {
      std::lock_guard lock(mtx);
      errorMsg = std::string("Failed to parse version data: ") + e.what();
      state.store(FetchState::ERROR);
    }
  }).detach();
}

std::vector<Utils::LibdragonCommit> Utils::LibdragonVersions::getCommits() const
{
  std::lock_guard lock(mtx);
  return commits;
}

std::string Utils::LibdragonVersions::getError() const
{
  std::lock_guard lock(mtx);
  return errorMsg;
}
