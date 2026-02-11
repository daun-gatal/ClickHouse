#pragma once

#include <optional>
#include <vector>
#include <memory>
#include <unordered_map>
#include <string_view>

#include <Core/Names.h>
#include <DataTypes/IDataType.h>
#include <Interpreters/ActionsDAG.h>
#include <Parsers/ASTExpressionList.h>
#include <Storages/IStorage_fwd.h>

namespace DB
{

class ASTAlterCommand;
class Context;
class WriteBuffer;
class ReadBuffer;

/// Represents set of actions which should be applied
/// to values from set of columns which satisfy predicate.
struct MutationCommand
{
    ASTPtr ast = {}; /// The AST of the whole command

    enum Type
    {
        EMPTY,     /// Not used.
        DELETE,
        UPDATE,
        MATERIALIZE_INDEX,
        MATERIALIZE_PROJECTION,
        MATERIALIZE_STATISTICS,
        READ_COLUMN, /// Read column and apply conversions (MODIFY COLUMN alter query).
        DROP_COLUMN,
        DROP_INDEX,
        DROP_PROJECTION,
        DROP_STATISTICS,
        MATERIALIZE_TTL,
        REWRITE_PARTS,
        RENAME_COLUMN,
        MATERIALIZE_COLUMN,
        APPLY_DELETED_MASK,
        APPLY_PATCHES,
        ALTER_WITHOUT_MUTATION, /// pure metadata command
    };

    Type type = EMPTY;

    /// WHERE part of mutation
    ASTPtr predicate = {};

    /// Columns with corresponding actions
    std::unordered_map<String, ASTPtr> column_to_update_expression = {};

    /// For MATERIALIZE INDEX and PROJECTION and STATISTICS
    String index_name = {};
    String projection_name = {};
    std::vector<String> statistics_columns = {};
    std::vector<String> statistics_types = {};

    /// For MATERIALIZE INDEX, UPDATE and DELETE.
    ASTPtr partition = {};

    /// For reads, drops and etc.
    String column_name = {};
    DataTypePtr data_type = {}; /// Maybe empty if we just want to drop column

    /// We need just clear column, not drop from metadata.
    bool clear = false;

    /// Column rename_to
    String rename_to = {};

    /// A version of mutation to which command corresponds.
    std::optional<UInt64> mutation_version = {};

    /// True if column is read by mutation command to apply patch.
    /// Required to distinguish read command used for MODIFY COLUMN.
    bool read_for_patch = false;

    /// If parse_alter_commands, than consider more Alter commands as mutation commands
    static std::optional<MutationCommand> parse(const ASTAlterCommand & command, bool parse_alter_commands = false, bool with_pure_metadata_commands = false);

    /// This command shouldn't stick with other commands
    bool isBarrierCommand() const;
    bool isPureMetadataCommand() const;
    bool isEmptyCommand() const;
    bool isDropOrRename() const;
    bool affectsAllColumns() const;
};

inline std::string_view enumToString(MutationCommand::Type type)
{
    switch (type)
    {
        case MutationCommand::EMPTY:                  return "EMPTY";
        case MutationCommand::DELETE:                  return "DELETE";
        case MutationCommand::UPDATE:                  return "UPDATE";
        case MutationCommand::MATERIALIZE_INDEX:       return "MATERIALIZE_INDEX";
        case MutationCommand::MATERIALIZE_PROJECTION:  return "MATERIALIZE_PROJECTION";
        case MutationCommand::MATERIALIZE_STATISTICS:  return "MATERIALIZE_STATISTICS";
        case MutationCommand::READ_COLUMN:             return "READ_COLUMN";
        case MutationCommand::DROP_COLUMN:             return "DROP_COLUMN";
        case MutationCommand::DROP_INDEX:              return "DROP_INDEX";
        case MutationCommand::DROP_PROJECTION:         return "DROP_PROJECTION";
        case MutationCommand::DROP_STATISTICS:         return "DROP_STATISTICS";
        case MutationCommand::MATERIALIZE_TTL:         return "MATERIALIZE_TTL";
        case MutationCommand::REWRITE_PARTS:           return "REWRITE_PARTS";
        case MutationCommand::RENAME_COLUMN:           return "RENAME_COLUMN";
        case MutationCommand::MATERIALIZE_COLUMN:      return "MATERIALIZE_COLUMN";
        case MutationCommand::APPLY_DELETED_MASK:      return "APPLY_DELETED_MASK";
        case MutationCommand::APPLY_PATCHES:           return "APPLY_PATCHES";
        case MutationCommand::ALTER_WITHOUT_MUTATION:  return "ALTER_WITHOUT_MUTATION";
    }
    return "Unknown";
}

/// Multiple mutation commands, possibly from different ALTER queries
class MutationCommands : public std::vector<MutationCommand>
{
public:
    boost::intrusive_ptr<ASTExpressionList> ast(bool with_pure_metadata_commands = false) const;

    void writeText(WriteBuffer & out, bool with_pure_metadata_commands) const;
    void readText(ReadBuffer & in, bool with_pure_metadata_commands);
    std::string toString(bool with_pure_metadata_commands) const;
    bool hasNonEmptyMutationCommands() const;

    bool hasAnyUpdateCommand() const;
    bool hasOnlyUpdateCommands() const;

    /// These set of commands contain barrier command and shouldn't
    /// stick with other commands. Commands from one set have already been validated
    /// to be executed without issues on the creation state.
    bool containBarrierCommand() const;
    NameSet getAllUpdatedColumns() const;
};

using MutationCommandsConstPtr = std::shared_ptr<MutationCommands>;

/// A pair of Actions DAG that is required to execute one step
/// of mutation and the name of filter column if it's a filtering step.
struct MutationActions
{
    ActionsDAG dag;
    String filter_column_name;
    bool project_input;
    std::optional<UInt64> mutation_version;
};

}
