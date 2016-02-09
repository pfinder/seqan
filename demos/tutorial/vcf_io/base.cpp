#include <seqan/sequence.h>

//![vcfRecord]
using namespace seqan;

class VcfRecord
{
public:
    __int32 rID;                          // CHROM
    __int32 beginPos;                     // POS
    CharString id;                        // ID
    CharString ref;                       // REF
    CharString alt;                       // ALT
    float qual;                           // QUAL
    CharString filter;                    // FILTER
    CharString info;                      // INFO
    CharString format;                    // FORMAT
    StringSet<CharString> genotypeInfos;  // <individual1> <individual2> ..

    // Constants for marking reference id and position as invalid.
    static const __int32 INVALID_REFID = -1;
    static const __int32 INVALID_POS = -1;
    // This function returns the float value for "invalid quality".
    static float MISSING_QUAL();
};
//![vcfRecord]

int main()
{
    return 0;
}
