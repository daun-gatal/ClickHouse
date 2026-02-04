#pragma once

#include <Analyzer/IQueryTreeNode.h>
#include <Core/QueryProcessingStage.h>
#include <Processors/QueryPlan/QueryPlan.h>
#include <Storages/MergeTree/ParallelReplicasReadingCoordinator.h>

namespace DB
{

std::pair<std::unique_ptr<QueryPlan>, bool> createLocalPlanForParallelReplicas(
    const QueryTreeNodePtr & query_tree,
    const Block & header,
    ContextPtr context,
    QueryProcessingStage::Enum processed_stage,
    ParallelReplicasReadingCoordinatorPtr coordinator,
    QueryPlanStepPtr read_from_merge_tree,
    size_t replica_number);
}
