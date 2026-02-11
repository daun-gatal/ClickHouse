#pragma once

#include <memory>
#include <unordered_map>
#include <Interpreters/SelectQueryOptions.h>

namespace DB
{

class IQueryTreeNode;
using QueryTreeNodePtr = std::shared_ptr<IQueryTreeNode>;

struct TemporaryTableHolder;
using TemporaryTableHolderPtr = std::shared_ptr<TemporaryTableHolder>;

using TableHolderToCTEMap = std::unordered_map<const TemporaryTableHolder *, QueryTreeNodePtr>;

TableHolderToCTEMap collectMaterializedCTEs(const QueryTreeNodePtr & node, const SelectQueryOptions & select_query_options);

}
