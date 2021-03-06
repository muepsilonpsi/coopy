#ifndef COOPY_MERGER
#define COOPY_MERGER

#include <coopy/OrderMerge.h>
#include <coopy/OrderResult.h>
#include <coopy/DataSheet.h>
#include <coopy/MergeOutput.h>
#include <coopy/CompareFlags.h>
#include <coopy/NameSniffer.h>
#include <coopy/EfficientMap.h>

#include <vector>
#include <set>

namespace coopy {
  namespace cmp {
    class Merger;
    class MergerState;
  }
}

class coopy::cmp::MergerState {
public:
  coopy::store::DataSheet& pivot;
  coopy::store::DataSheet& local;
  coopy::store::DataSheet& remote;
  const OrderResult& nrow_local;
  const OrderResult& nrow_remote;
  const OrderResult& ncol_local;
  const OrderResult& ncol_remote;
  Patcher& output;
  const CompareFlags& flags;
  coopy::store::NameSniffer& local_names;
  coopy::store::NameSniffer& remote_names;
  bool allIdentical;

  MergerState(coopy::store::DataSheet& pivot,
	      coopy::store::DataSheet& local,
	      coopy::store::DataSheet& remote,
	      const OrderResult& nrow_local,
	      const OrderResult& nrow_remote,
	      const OrderResult& ncol_local,
	      const OrderResult& ncol_remote,
	      Patcher& output,
	      const CompareFlags& flags,
	      coopy::store::NameSniffer& local_names,
	      coopy::store::NameSniffer& remote_names) :
    pivot(pivot), local(local), remote(remote),
    nrow_local(nrow_local), nrow_remote(nrow_remote),
    ncol_local(ncol_local), ncol_remote(ncol_remote),
    output(output),
    flags(flags),
    local_names(local_names), remote_names(remote_names) {
      allIdentical = false;
  }
};

class coopy::cmp::Merger {

public:
  Merger() {
  }

  bool merge(MergerState& state);

  bool mergeRow(coopy::store::DataSheet& pivot, 
		coopy::store::DataSheet& local, 
		coopy::store::DataSheet& remote,
		MatchUnit& row_unit, Patcher& output,
		const CompareFlags& flags,
		std::vector<coopy::cmp::RowChange>& rc);

private:
  OrderMerge row_merge;
  OrderMerge col_merge;
  int conflicts;
  std::vector<std::string> lastAddress;
  std::vector<std::string> lastAction;
  std::vector<std::string> names;
  std::set<std::string> filtered_names;
  int last_local_row;
  int last_local_row_marked;
  int bottom_local_row;
  RowChange lastRowChange;
  bool had_row;
  bool had_foreign_row;
  efficient_map<std::string,int> include_column;
  efficient_map<std::string,int> exclude_column;

  int current_row;
  int last_row;
  int addition;
  bool allGone;
  //SheetSchema defaultSheetSchema;
  //SheetSchema *pivotSheetSchema;
  //SheetSchema *localSheetSchema;
  //SheetSchema *remoteSheetSchema;

};

#endif

