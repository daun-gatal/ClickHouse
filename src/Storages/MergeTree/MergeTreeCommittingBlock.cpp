#include <Storages/MergeTree/MergeTreeCommittingBlock.h>
#include <Storages/StorageMergeTree.h>
#include <IO/ReadBufferFromString.h>
#include <IO/WriteBufferFromString.h>

namespace DB
{

namespace ErrorCodes
{
    extern const int UNKNOWN_FORMAT_VERSION;
}

PlainCommittingBlockHolder::PlainCommittingBlockHolder(CommittingBlock block_, StorageMergeTree & storage_)
    : block(std::move(block_)), storage(storage_)
{
}

PlainCommittingBlockHolder::~PlainCommittingBlockHolder()
{
    storage.removeCommittingBlock(block);
}

template <class Enum>
int64_t toIntChecked(Enum value)
{
    int64_t underlying = static_cast<std::underlying_type_t<Enum>>(value);

    /// Validate the value is a known enum member
    switch (value)
    {
        case Enum::Unknown:
        case Enum::NewPart:
        case Enum::Update:
        case Enum::Mutation:
            return underlying;
    }

    throw Exception(ErrorCodes::UNKNOWN_FORMAT_VERSION, "Unknown CommittingBlock::Op value {}", underlying);
}

template <class Enum>
Enum fromIntChecked(int64_t underlying)
{
    switch (static_cast<Enum>(underlying))
    {
        case Enum::Unknown:
        case Enum::NewPart:
        case Enum::Update:
        case Enum::Mutation:
            return static_cast<Enum>(underlying);
    }

    throw Exception(ErrorCodes::UNKNOWN_FORMAT_VERSION, "Unknown CommittingBlock::Op value {}", underlying);
}

static void serializeCommittingBlockOpToBuffer(CommittingBlock::Op op, WriteBuffer & out)
{
    out << "operation: " << toIntChecked(op) << "\n";
}

static CommittingBlock::Op deserializeCommittingBlockOpFromBuffer(ReadBuffer & in)
{
    int64_t op;
    in >> "operation: " >> op >> "\n";
    return fromIntChecked<CommittingBlock::Op>(op);
}

std::string serializeCommittingBlockOpToString(CommittingBlock::Op op)
{
    WriteBufferFromOwnString out;
    serializeCommittingBlockOpToBuffer(op, out);
    return out.str();
}

CommittingBlock::Op deserializeCommittingBlockOpFromString(const std::string & representation)
{
    try
    {
        if (!representation.starts_with("operation"))
            return CommittingBlock::Op::Unknown;

        ReadBufferFromString in(representation);
        auto committing_block_op = deserializeCommittingBlockOpFromBuffer(in);

        assertEOF(in);
        return committing_block_op;
    }
    catch (...)
    {
        return CommittingBlock::Op::Unknown;
    }
}

}
