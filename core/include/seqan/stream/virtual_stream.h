// ==========================================================================
//                 SeqAn - The Library for Sequence Analysis
// ==========================================================================
// Copyright (c) 2006-2013, Knut Reinert, FU Berlin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Knut Reinert or the FU Berlin nor the names of
//       its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL KNUT REINERT OR THE FU BERLIN BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// ==========================================================================
// Author: David Weese <david.weese@fu-berlin.de>
// ==========================================================================
// Virtual stream class to automatically compress/decompress files/streams.
// It adapts the zipstream and bzip2stream classes (Jonathan de Halleux, 2003)
// http://www.codeproject.com/Articles/4457/zipstream-bzip2stream-iostream-wrappers-for-the-zl
// ==========================================================================

#ifndef SEQAN_STREAM_VIRTUAL_STREAM_
#define SEQAN_STREAM_VIRTUAL_STREAM_

#if SEQAN_HAS_ZLIB
#include "zipstream/zipstream.h"
#include "zipstream/bgzfstream.h"
#endif

#if SEQAN_HAS_BZIP2
#include "zipstream/bzip2stream.h"
#endif

namespace seqan {

// ============================================================================
// Tags, Enums
// ============================================================================

#if SEQAN_HAS_ZLIB

template <typename Elem, typename Tr, typename ElemA, typename ByteT, typename ByteAT>
struct Value<basic_bgzf_istream<Elem, Tr, ElemA, ByteT, ByteAT> > :
    Value<std::basic_istream<Elem, Tr> > {};

template <typename Elem, typename Tr, typename ElemA, typename ByteT, typename ByteAT>
struct Position<basic_bgzf_istream<Elem, Tr, ElemA, ByteT, ByteAT> > :
    Position<std::basic_istream<Elem, Tr> > {};


template <typename Elem, typename Tr, typename ElemA, typename ByteT, typename ByteAT>
struct Value<basic_bgzf_ostream<Elem, Tr, ElemA, ByteT, ByteAT> > :
    Value<std::basic_ostream<Elem, Tr> > {};

template <typename Elem, typename Tr, typename ElemA, typename ByteT, typename ByteAT>
struct Position<basic_bgzf_ostream<Elem, Tr, ElemA, ByteT, ByteAT> > :
    Position<std::basic_ostream<Elem, Tr> > {};


template <typename Elem, typename Tr, typename ElemA, typename ByteT, typename ByteAT>
SEQAN_CONCEPT_IMPL((basic_bgzf_istream<Elem, Tr, ElemA, ByteT, ByteAT>), (InputStreamConcept));

template <typename Elem, typename Tr, typename ElemA, typename ByteT, typename ByteAT>
SEQAN_CONCEPT_IMPL((basic_bgzf_ostream<Elem, Tr, ElemA, ByteT, ByteAT>), (OutputStreamConcept));

#endif

// --------------------------------------------------------------------------
// TagList CompressedFileTypes
// --------------------------------------------------------------------------

typedef
#if SEQAN_HAS_ZLIB
    TagList<BgzfFile,
    TagList<GZFile,
#endif
#if SEQAN_HAS_BZIP2
    TagList<BZ2File,
#endif
    TagList<Nothing>
#if SEQAN_HAS_BZIP2
    >
#endif
#if SEQAN_HAS_ZLIB
    >
    >
#endif
    CompressedFileTypes;  // if TagSelector is set to -1, the file format is auto-detected

// ============================================================================
// Metafunctions
// ============================================================================

// --------------------------------------------------------------------------
// Metafunction VirtualStreamSwitch_
// --------------------------------------------------------------------------

template <typename TValue, typename TDirection, typename TFormatTag>
struct VirtualStreamSwitch_
{
    typedef Nothing Type;
};

#if SEQAN_HAS_ZLIB
template <typename TValue>
struct VirtualStreamSwitch_<TValue, Input, GZFile>
{
    typedef zlib_stream::basic_zip_istream<TValue> Type;
};

template <typename TValue>
struct VirtualStreamSwitch_<TValue, Output, GZFile>
{
    typedef zlib_stream::basic_zip_ostream<TValue> Type;
};

template <typename TValue>
struct VirtualStreamSwitch_<TValue, Input, BgzfFile>
{
    typedef basic_bgzf_istream<TValue> Type;
};

template <typename TValue>
struct VirtualStreamSwitch_<TValue, Output, BgzfFile>
{
    typedef basic_bgzf_ostream<TValue> Type;
};

#endif

#if SEQAN_HAS_BZIP2

template <typename TValue>
struct VirtualStreamSwitch_<TValue, Input, BZ2File>
{
    typedef bzip2_stream::basic_bzip2_istream<TValue> Type;
};

template <typename TValue>
struct VirtualStreamSwitch_<TValue, Output, BZ2File>
{
    typedef bzip2_stream::basic_bzip2_ostream<TValue> Type;
};
#endif

// ==========================================================================
// Classes
// ==========================================================================

// --------------------------------------------------------------------------
// Class VirtualStreamFactoryContext_
// --------------------------------------------------------------------------

template <typename TVirtualStream>
struct VirtualStreamFactoryContext_;

// --------------------------------------------------------------------------
// Class VirtualStreamContext_
// --------------------------------------------------------------------------

// a compressed stream lives in the VirtualStreamContext_ and provides a basic_streambuf

// generic subclass with virtual destructor
template <typename TValue, typename TDirection, typename TFormatTag = void>
struct VirtualStreamContext_:
    VirtualStreamContext_<TValue, TDirection>
{
    typename VirtualStreamSwitch_<TValue, TDirection, TFormatTag>::Type stream;

    template <typename TObject>
    VirtualStreamContext_(TObject &object):
        stream(object)
    {
        this->streamBuf = stream.rdbuf();
    }
};

// special case: no compression, we simply forward the file stream
template <typename TValue, typename TDirection>
struct VirtualStreamContext_<TValue, TDirection, Nothing>:
    VirtualStreamContext_<TValue, TDirection>
{
    template <typename TObject>
    VirtualStreamContext_(TObject &object)
    {
        this->streamBuf = object.rdbuf();
    }
};

// base class
template <typename TValue, typename TDirection>
struct VirtualStreamContext_<TValue, TDirection>
{
    std::basic_streambuf<TValue> *streamBuf;
    VirtualStreamContext_(): streamBuf() {}
    virtual ~VirtualStreamContext_() {}
};

// --------------------------------------------------------------------------
// Class VirtualStream
// --------------------------------------------------------------------------

// The VirtualStream class handles a file or input stream and auto-detects data
// compression from file name or stream.
// We inherit from std::basic_Xstream to provide the convenient stream interface.
template <typename TValue, typename TDirection, typename TTraits = std::char_traits<TValue> >
class VirtualStream: public BasicStream<TValue, TDirection, TTraits>::Type
{
public:
    typedef typename BasicStream<TValue, TDirection, TTraits>::Type  TStream;      // the stream base class we expose
    typedef std::fstream                                    TFile;                  // if a real file should be opened
    typedef BufferedStream<TStream, TDirection>             TBufferedStream;        // if input stream is not buffered
    typedef std::basic_streambuf<TValue>                    TStreamBuffer;          // the streambuf to use
    typedef VirtualStreamContext_<TValue, TDirection>       TVirtualStreamContext;  // the owner of the streambuf

    TFile                   file;
    TBufferedStream         bufferedStream;
    TStreamBuffer           *streamBuf;
    TVirtualStreamContext   *context;

    VirtualStream():
        TStream(NULL),
        streamBuf(),
        context()
    {}

    VirtualStream(TStreamBuffer &streamBuf):
        TStream(NULL),
        streamBuf(streamBuf),
        context()
    {}

    VirtualStream(TStream &stream):
        TStream(NULL),
        streamBuf(),
        context()
    {
        open(*this, stream);
    }

    VirtualStream(const char *fileName, int openMode):
        TStream(NULL),
        streamBuf(),
        context()
    {
        open(*this, fileName, openMode);
    }

    ~VirtualStream()
    {
        close(*this);
    }


    operator TStreamBuffer*() const
    {
        return streamBuf;
    }

    operator bool() const
    {
        return streamBuf != NULL;
    }

    void _init()
    {
        this->init(streamBuf);
    }

    TStreamBuffer* rdbuf() const
    {
        return streamBuf;
    }
};

// ----------------------------------------------------------------------------
// Metafunction Value
// ----------------------------------------------------------------------------

template <typename TValue, typename TDirection>
struct Value<VirtualStream<TValue, TDirection> >
{
    typedef TValue Type;
};

// ----------------------------------------------------------------------------
// Metafunction Position
// ----------------------------------------------------------------------------

template <typename TValue, typename TDirection>
struct Position<VirtualStream<TValue, TDirection> >:
    Position<typename VirtualStream<TValue, TDirection>::TFile> {};

// ----------------------------------------------------------------------------
// Metafunction Iterator<Standard>
// ----------------------------------------------------------------------------

template <typename TValue, typename TDirection>
struct Iterator<VirtualStream<TValue, TDirection>, TDirection>
{
    typedef Iter<VirtualStream<TValue, TDirection>, StreamIterator<TDirection> > Type;
};

// --------------------------------------------------------------------------
// Metafunction DefaultOpenMode<Input>
// --------------------------------------------------------------------------

template <typename TValue, typename TDummy>
struct DefaultOpenMode<VirtualStream<TValue, Input>, TDummy>
{
    enum { VALUE = OPEN_RDONLY };
};

// --------------------------------------------------------------------------
// Metafunction DefaultOpenMode<Output>
// --------------------------------------------------------------------------

template <typename TValue, typename TDummy>
struct DefaultOpenMode<VirtualStream<TValue, Output>, TDummy>
{
    enum { VALUE = OPEN_WRONLY | OPEN_CREATE };
};

// ----------------------------------------------------------------------------
// Concepts
// ----------------------------------------------------------------------------

template <typename TValue>
SEQAN_CONCEPT_IMPL((VirtualStream<TValue, Input>), (InputStreamConcept));

template <typename TValue>
SEQAN_CONCEPT_IMPL((VirtualStream<TValue, Output>), (OutputStreamConcept));

template <typename TValue>
SEQAN_CONCEPT_IMPL((VirtualStream<TValue, Bidirectional>), (BidirectionalStreamConcept));

// --------------------------------------------------------------------------
// Class VirtualStreamFactoryContext_
// --------------------------------------------------------------------------

template <typename TValue, typename TDirection>
struct VirtualStreamFactoryContext_<VirtualStream<TValue, TDirection> >
{
    typedef VirtualStream<TValue, TDirection>       TVirtualStream;
    typedef typename TVirtualStream::TStream        TStream;

    TStream &stream;
    VirtualStreamFactoryContext_(TStream &stream):
        stream(stream) {}
};

template <typename TVirtualStream>
struct Value<VirtualStreamFactoryContext_<TVirtualStream> >
{
    typedef typename TVirtualStream::TVirtualStreamContext *Type;
};

// ============================================================================
// Functions
// ============================================================================

// --------------------------------------------------------------------------
// Function tagApply()
// --------------------------------------------------------------------------

template <typename TValue, typename TDirection, typename TFormat>
inline VirtualStreamContext_<TValue, TDirection>*
tagApply(VirtualStreamFactoryContext_<VirtualStream<TValue, TDirection> > &ctx, Tag<TFormat>)
{
    return new VirtualStreamContext_<TValue, TDirection, Tag<TFormat> >(ctx.stream);
}


template <typename TContext>
inline typename Value<TContext>::Type
tagApply(TContext &, TagSelector<>)
{
    return typename Value<TContext>::Type();
}

template <typename TContext, typename TTagList>
inline typename Value<TContext>::Type
tagApply(TContext &ctx, TagSelector<TTagList> &format)
{
    typedef typename TTagList::Type TFormatTag;

    if (isEqual(format, TFormatTag()))
        return tagApply(ctx, TFormatTag());

    return tagApply(ctx, static_cast<typename TagSelector<TTagList>::Base &>(format));
}

// --------------------------------------------------------------------------
// Function guessFormat()
// --------------------------------------------------------------------------

// read first bytes of a file/stream and compare with file format's magic header
template <typename TStream, typename TFormat_>
inline bool
guessFormatFromStream(TStream &istream, Tag<TFormat_>)
{
    typedef Tag<TFormat_> TFormat;

    SEQAN_ASSERT(istream.good());

    if (MagicHeader<TFormat>::VALUE == NULL)
        return true;

    bool match = true;

    // check magic header
    unsigned i;
    for (i = 0; i != sizeof(MagicHeader<TFormat>::VALUE) / sizeof(char); ++i)
    {
        int c = (int)istream.get();
        if (c != MagicHeader<TFormat>::VALUE[i])
        {
            match = false;
            if (c != EOF)
                ++i;
            break;
        }
    }

    // unget all read characters
    for (; i > 0; --i)
        istream.unget();

    SEQAN_ASSERT(istream.good());

    return match;
}

// ----------------------------------------------------------------------------
// _guessFormat wrapper
// ----------------------------------------------------------------------------

template <typename TValue, typename TStream, typename TCompressionType>
inline bool _guessFormat(VirtualStream<TValue, Input> &, TStream &fileStream, TCompressionType &compressionType)
{
    return guessFormatFromStream(fileStream, compressionType);
}

template <typename TValue, typename TStream, typename TCompressionType>
inline bool _guessFormat(VirtualStream<TValue, Output> &, TStream &, TCompressionType &)
{
    return true;
}

// --------------------------------------------------------------------------
// Function open()
// --------------------------------------------------------------------------

template <typename TValue, typename TDirection, typename TStream, typename TCompressionType>
inline bool
open(VirtualStream<TValue, TDirection> &stream, TStream &fileStream, TCompressionType & compressionType)
{
    typedef VirtualStream<TValue, TDirection> TVirtualStream;
    typedef typename TVirtualStream::TBufferedStream TBufferedStream;

    // peek the first character to initialize the underlying streambuf (for in_avail)
    fileStream.rdbuf()->sgetc();

    if (IsSameType<TDirection, Input>::VALUE &&
        !IsSameType<TStream, TBufferedStream>::VALUE &&
        fileStream.rdbuf()->in_avail() < 2)
    {
        stream.bufferedStream.setStream(fileStream);
        return open(stream, stream.bufferedStream, compressionType);
    }

    VirtualStreamFactoryContext_<TVirtualStream> ctx(fileStream);

    // try to detect/verify format
    if (!_guessFormat(stream, fileStream, compressionType))
        return false;

    // create a new (un)zipper buffer
    stream.context = tagApply(ctx, compressionType);
    if (stream.context == NULL)
        return false;

    SEQAN_ASSERT(stream.context->streamBuf != NULL);
    stream.streamBuf = stream.context->streamBuf;

    // reset our outer stream interface
    stream._init();
    return true;
}

template <typename TValue, typename TDirection, typename TStream, typename TCompressionType>
inline bool
open(VirtualStream<TValue, TDirection> &stream, TStream &fileStream, TCompressionType const & compressionType)
{
    TCompressionType ct = compressionType;
    return open(stream, fileStream, ct);
}

template <typename TValue, typename TStream>
inline bool
open(VirtualStream<TValue, Input> &stream, TStream &fileStream)
{
    // detect compression type from file extension
    TagSelector<CompressedFileTypes> compressionType;
    return open(stream, fileStream, compressionType);
}

template <typename TValue, typename TDirection>
inline bool
open(VirtualStream<TValue, TDirection> &stream, const char *fileName, int openMode)
{
    typedef VirtualStream<TValue, TDirection> TVirtualStream;

    if (!open(stream.file, fileName, openMode | std::ios::binary))
        return false;

    // detect compression type from file extension
    TagSelector<CompressedFileTypes> fileType;
    guessFormatFromFilename(fileName, fileType);

    VirtualStreamFactoryContext_<TVirtualStream> ctx(stream.file);

    // create a new (un)zipper buffer
    stream.context = tagApply(ctx, fileType);
    if (stream.context == NULL)
    {
        close(stream.file);
        return false;
    }
    stream.streamBuf = stream.context->streamBuf;

    // reset our outer stream interface
    stream._init();
    return true;
}

// --------------------------------------------------------------------------
// Function close()
// --------------------------------------------------------------------------

template <typename TValue, typename TDirection>
inline bool
close(VirtualStream<TValue, TDirection> &stream)
{
    delete stream.context;
    stream.context = NULL;
    stream.streamBuf = NULL;
    return close(stream.file);
}

}  // namespace seqan

#endif  // #ifndef SEQAN_STREAM_VIRTUAL_STREAM_
