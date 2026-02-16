#ifndef FW_JOURNAL_STATE_H
#define FW_JOURNAL_STATE_H

#include <string>
#include <vector>

#include <gf2/core/TypeTraits.h>

#include "Date.h"

namespace fw {

  struct JournalEntryState {
    Date date;
    std::string message;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<JournalEntryState, Archive>& state)
  {
    return ar | state.date | state.message;
  }

  struct JournalState {
    std::vector<JournalEntryState> entries;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<JournalState, Archive>& state)
  {
    return ar | state.entries;
  }

}

#endif // FW_JOURNAL_STATE_H
