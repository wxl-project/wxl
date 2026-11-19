// tools/package/zip-epoch.cs
//
// Sets the time of every entry of a zip archive to the zip epoch, 1980-01-01
// 00:00, in place: the DOS time and date of each local header and of each
// header of the central directory. Nothing else of the archive is touched --
// the entries keep their order, their compressed bytes and their checksums.
//
//     dotnet run --file tools/package/zip-epoch.cs -- <archive>
//
// pack.cmake runs it on the package dotnet pack wrote: NuGet stamps the parts it
// makes (the manifest, _rels, [Content_Types].xml, the core properties) with the
// time of the pack and every file with its own modification time, and its
// Deterministic and DeterministicTimestamp properties change neither, so without
// it two packs of the same files are different bytes. The zip epoch is the
// least time the format holds, and it depends on nothing of the build.
//
// An archive this program does not read in full -- an extra field that may
// carry a time of its own (any other than Zip64), a split archive, a header out
// of place -- stops it, and nothing is written.

#:property PublishAot=false

using System.Buffers.Binary;

if (args.Length != 1)
{
    Console.Error.WriteLine("usage: dotnet run --file zip-epoch.cs -- <archive>");
    return 2;
}

try
{
    var count = ZipEpoch.Apply(args[0]);
    Console.WriteLine($"zip-epoch: {count} entries of {args[0]} at 1980-01-01 00:00");
    return 0;
}
catch (InvalidDataException e)
{
    Console.Error.WriteLine($"zip-epoch: {args[0]}: {e.Message}");
    return 1;
}

static class ZipEpoch
{
    const uint LocalHeader = 0x04034b50;
    const uint CentralHeader = 0x02014b50;
    const uint EndOfCentralDirectory = 0x06054b50;
    const uint Zip64EndOfCentralDirectory = 0x06064b50;
    const uint Zip64Locator = 0x07064b50;
    const ushort Zip64Extra = 0x0001;

    // DOS time 00:00:00 and date 1980-01-01: (year - 1980) << 9 | month << 5 | day.
    const ushort EpochTime = 0;
    const ushort EpochDate = (0 << 9) | (1 << 5) | 1;

    public static int Apply(string path)
    {
        var bytes = File.ReadAllBytes(path);
        var span = bytes.AsSpan();

        var (entries, directory, directorySize) = ReadEnd(span);
        if (directory + directorySize > (ulong)bytes.Length)
            throw new InvalidDataException("the central directory lies past the end of the archive");

        var at = (int)directory;
        for (ulong i = 0; i < entries; i++)
        {
            if (Read32(span, at) != CentralHeader)
                throw new InvalidDataException($"no header of the central directory at {at}");
            var nameLength = Read16(span, at + 28);
            var extraLength = Read16(span, at + 30);
            var commentLength = Read16(span, at + 32);
            var disk = Read16(span, at + 34);
            ulong local = Read32(span, at + 42);
            var extra = span.Slice(at + 46 + nameLength, extraLength);
            if (local == uint.MaxValue)
                local = Zip64LocalOffset(extra, span.Slice(at, 46));
            else
                CheckExtra(extra);
            if (disk != 0)
                throw new InvalidDataException("the archive is split across disks");

            Write16(span, at + 12, EpochTime);
            Write16(span, at + 14, EpochDate);

            if (local + 30 > (ulong)bytes.Length || Read32(span, (int)local) != LocalHeader)
                throw new InvalidDataException($"no local header at {local}");
            var l = (int)local;
            CheckExtra(span.Slice(l + 30 + Read16(span, l + 26), Read16(span, l + 28)));
            Write16(span, l + 10, EpochTime);
            Write16(span, l + 12, EpochDate);

            at += 46 + nameLength + extraLength + commentLength;
        }

        File.WriteAllBytes(path, bytes);
        return (int)entries;
    }

    // The number of entries and the place and size of the central directory, from
    // the end record -- or from the Zip64 end record its locator names, when a
    // field of the end record is at its maximum.
    static (ulong entries, ulong directory, ulong size) ReadEnd(ReadOnlySpan<byte> span)
    {
        var end = -1;
        for (var at = span.Length - 22; at >= Math.Max(0, span.Length - 22 - ushort.MaxValue); at--)
        {
            if (Read32(span, at) == EndOfCentralDirectory && at + 22 + Read16(span, at + 20) == span.Length)
            {
                end = at;
                break;
            }
        }
        if (end < 0)
            throw new InvalidDataException("no end of the central directory");

        ulong entries = Read16(span, end + 10);
        ulong size = Read32(span, end + 12);
        ulong directory = Read32(span, end + 16);
        if (entries != ushort.MaxValue && size != uint.MaxValue && directory != uint.MaxValue)
            return (entries, directory, size);

        var locator = end - 20;
        if (locator < 0 || Read32(span, locator) != Zip64Locator)
            throw new InvalidDataException("the end record asks for Zip64 and no locator precedes it");
        var record = BinaryPrimitives.ReadUInt64LittleEndian(span.Slice(locator + 8));
        if (record + 56 > (ulong)span.Length || Read32(span, (int)record) != Zip64EndOfCentralDirectory)
            throw new InvalidDataException($"no Zip64 end record at {record}");
        var r = (int)record;
        return (BinaryPrimitives.ReadUInt64LittleEndian(span.Slice(r + 32)),
                BinaryPrimitives.ReadUInt64LittleEndian(span.Slice(r + 48)),
                BinaryPrimitives.ReadUInt64LittleEndian(span.Slice(r + 40)));
    }

    // The offset of the local header in the Zip64 extra field: after the sizes the
    // central header also has at their maximum, in the order the format gives.
    static ulong Zip64LocalOffset(ReadOnlySpan<byte> extra, ReadOnlySpan<byte> header)
    {
        CheckExtra(extra);
        for (var at = 0; at + 4 <= extra.Length; at += 4 + Read16(extra, at + 2))
        {
            if (Read16(extra, at) != Zip64Extra)
                continue;
            var field = at + 4;
            if (Read32(header, 24) == uint.MaxValue) field += 8;
            if (Read32(header, 20) == uint.MaxValue) field += 8;
            return BinaryPrimitives.ReadUInt64LittleEndian(extra.Slice(field));
        }
        throw new InvalidDataException("the offset of a local header is in a Zip64 field the entry does not have");
    }

    // Only Zip64 fields: any other (the NTFS or the extended timestamp field)
    // could carry a time this program does not set.
    static void CheckExtra(ReadOnlySpan<byte> extra)
    {
        for (var at = 0; at < extra.Length; at += 4 + Read16(extra, at + 2))
        {
            if (at + 4 > extra.Length || at + 4 + Read16(extra, at + 2) > extra.Length)
                throw new InvalidDataException("an extra field runs past its entry");
            if (Read16(extra, at) != Zip64Extra)
                throw new InvalidDataException($"an entry has the extra field 0x{Read16(extra, at):x4}, which may hold a time");
        }
    }

    static ushort Read16(ReadOnlySpan<byte> span, int at) => BinaryPrimitives.ReadUInt16LittleEndian(span.Slice(at));
    static uint Read32(ReadOnlySpan<byte> span, int at) => BinaryPrimitives.ReadUInt32LittleEndian(span.Slice(at));
    static void Write16(Span<byte> span, int at, ushort value) => BinaryPrimitives.WriteUInt16LittleEndian(span.Slice(at), value);
}
