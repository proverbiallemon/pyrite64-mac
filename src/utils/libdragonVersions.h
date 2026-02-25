/**
* @copyright 2026 - Max Bebök
* @license MIT
*/
#pragma once
#include <string>
#include <vector>
#include <atomic>
#include <mutex>

namespace Utils
{
  struct LibdragonCommit {
    std::string sha;      // full 40-char hash
    std::string message;  // first line of commit message
    std::string date;     // e.g. "Feb 24"
  };

  enum class FetchState { IDLE, FETCHING, DONE, ERROR };

  class LibdragonVersions
  {
    public:
      void fetchCommits();
      FetchState getState() const { return state.load(); }
      std::vector<LibdragonCommit> getCommits() const;
      std::string getError() const;

    private:
      std::atomic<FetchState> state{FetchState::IDLE};
      std::vector<LibdragonCommit> commits{};
      std::string errorMsg{};
      mutable std::mutex mtx{};
  };
}
