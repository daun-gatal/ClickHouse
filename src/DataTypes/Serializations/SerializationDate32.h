#pragma once

#include <DataTypes/Serializations/SerializationNumber.h>
#include <DataTypes/Serializations/SerializationObjectPool.h>
#include <Common/DateLUT.h>

namespace DB
{
class SerializationDate32 final : public SerializationNumber<Int32>
{
private:
    explicit SerializationDate32(const DateLUTImpl & time_zone_ = DateLUT::instance());

public:
    static SerializationPtr create(const DateLUTImpl & time_zone_ = DateLUT::instance())
    {
        // Note: time_zone_ is captured by reference because DateLUTImpl is not copyable.
        // This is safe because: (1) time_zone_ is always a reference to a DateLUT singleton,
        // and (2) the factory lambda is called immediately within getOrCreate() if needed.
        return SerializationObjectPool::instance().getOrCreate(
            "Date32",
            [&time_zone_] { return SerializationPtr(new SerializationDate32(time_zone_)); });
    }

    ~SerializationDate32() override;

    String getName() const override;

    void serializeText(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings &) const override;
    void deserializeWholeText(IColumn & column, ReadBuffer & istr, const FormatSettings &) const override;
    bool tryDeserializeWholeText(IColumn & column, ReadBuffer & istr, const FormatSettings &) const override;
    void serializeTextEscaped(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings &) const override;
    void deserializeTextEscaped(IColumn & column, ReadBuffer & istr, const FormatSettings &) const override;
    bool tryDeserializeTextEscaped(IColumn & column, ReadBuffer & istr, const FormatSettings &) const override;
    void serializeTextQuoted(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings &) const override;
    void deserializeTextQuoted(IColumn & column, ReadBuffer & istr, const FormatSettings &) const override;
    bool tryDeserializeTextQuoted(IColumn & column, ReadBuffer & istr, const FormatSettings &) const override;
    void serializeTextJSON(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings &) const override;
    void deserializeTextJSON(IColumn & column, ReadBuffer & istr, const FormatSettings &) const override;
    bool tryDeserializeTextJSON(IColumn & column, ReadBuffer & istr, const FormatSettings &) const override;
    void serializeTextCSV(const IColumn & column, size_t row_num, WriteBuffer & ostr, const FormatSettings &) const override;
    void deserializeTextCSV(IColumn & column, ReadBuffer & istr, const FormatSettings & settings) const override;
    bool tryDeserializeTextCSV(IColumn & column, ReadBuffer & istr, const FormatSettings & settings) const override;

protected:
    const DateLUTImpl & time_zone;
};
}
