// All active methods/inputs/assertions from the pinned original; reviewed translation.
#include "../../../../../support/OriginalBaseTestSupport.hpp"
using namespace minima;
namespace {
int assertions=0;
void require(bool value){++assertions;if(!value)throw std::runtime_error("MiniDataTests assertion "+std::to_string(assertions));}
// Java autoboxes primitive results. Native values are materialized as owned
// objects for the same non-null smoke check; no stronger equality is substituted.
template<class T> void notNull(const T& value){auto boxed=std::make_unique<T>(value);require(bool(boxed));}

    static void testValueBytes() {
        MiniData i=MiniData::fromHex("0x1388");
        MiniData j=MiniData::fromHex("0xFFFF");
        MiniData k=MiniData::fromHex("5475746f7269616c73706f69674");
        std::array<MiniData,2> mData={j,k};
        std::array<MiniData,3> mDataCompare={i,j,k};
        std::cout << ("i value is ") << (i) << '\n';
        std::cout << ("j value is ") << (j) << '\n';
        std::cout << ("k value is ") << (k) << '\n';
        std::cout << ("mData value is ") << (arrayText(mData)) << '\n';
        std::cout << ("mDataCompare value is ") << (arrayText(mDataCompare)) << '\n';
        notNull(i.getLength());
        std::cout << ("i.getLength()") << (i) << (" resolves as ") << (i.getLength()) << '\n';
        notNull(j.getLength());
        std::cout << ("i.getLength()") << (j) << (" resolves as ") << (j.getLength()) << '\n';
        notNull(k.getLength());
        std::cout << ("i.getLength()") << (k) << (" resolves as ") << (k.getLength()) << '\n';
        notNull(i.bytes());
        std::cout << ("i.getData()") << (i) << (" resolves as ") << (i.bytes()) << '\n';
        notNull(i.getDataValue());
        std::cout << ("i.getDataValue()") << (i) << (" resolves as ") << (i.getDataValue()) << '\n';
        notNull(i.getDataValueDecimal());
        std::cout << ("i.getDataValueDecimal()") << (i) << (" resolves as ") << (i.getDataValueDecimal()) << '\n';
        notNull(i.to0xString());
        std::cout << ("i.to0xString()") << (i) << (" resolves as ") << (i.to0xString()) << '\n';
        notNull(i.to0xString());
        std::cout << ("i.toString()") << (i) << (" resolves as ") << (i.to0xString()) << '\n';
        notNull(mData[0].compare(k));
        std::cout << ("mData[0].compare(k) resolves as ") << (mData[0].compare(k)) << '\n';
        notNull(MiniData::getRandomData(1));
        std::cout << ("i.getRandomData(1)") << (i) << (" resolves as ") << (MiniData::getRandomData(1)) << '\n';
        notNull(i.shiftr(64));
        std::cout << ("i.shiftr(64) i:") << (i) << (" resolves as ") << (i.shiftr(64)) << '\n';
        notNull(i.shiftl(64));
        std::cout << ("i.shiftl(64) i:") << (i) << (" resolves as ") << (i.shiftl(64)) << '\n';
        notNull(i.concat(j));
        std::cout << ("i.concat(j) i:") << (i) << (" j:") << (j) << (" resolves as ") << (i.concat(j)) << '\n';
        notNull(i.to0xString(1));
        std::cout << ("i.to0xString(1)") << (i) << (" resolves as ") << (i.to0xString(1)) << '\n';
        notNull(i.to0xString(10000));
        std::cout << ("i.to0xString(10000)") << (i) << (" resolves as ") << (i.to0xString(10000)) << '\n';
        require(!(i.isEqual(j)));
        std::cout << ("i.equals(j) i:") << (i) << (" j:") << (j) << (" resolves as ") << (i.isEqual(j)) << '\n';
        require((j.isEqual(j)));
        std::cout << ("j.equals(j) j:") << (j) << (" j:") << (j) << (" resolves as ") << (j.isEqual(j)) << '\n';
        require(!(i.isEqual(j)));
        std::cout << ("i.isEqual(j)) i:") << (i) << (" j:") << (j) << (" resolves as ") << (i.isEqual(j)) << '\n';
        require(!(mData[1].isEqual(mDataCompare[1])));
        std::cout << ("mData[1].isEqual(mDataCompare[1]) mData[1]:") << (mData[1]) << (" mDataCompare[1]:") << (mDataCompare[1]) << (" resolves as ") << (mData[1].isEqual(mDataCompare[1])) << '\n';
        require((i.isLess(j)));
        std::cout << ("i.isLess(j) i:") << (i) << (" j:") << (j) << (" resolves as ") << (i.isLess(j)) << '\n';
        require(!(j.isLess(i)));
        std::cout << ("j.isLess(i) i:") << (i) << (" j:") << (j) << (" resolves as ") << (j.isLess(i)) << '\n';
        require(!(j.isLessEqual(i)));
        std::cout << ("j.isLessEqual(i) i:") << (i) << (" j:") << (j) << (" resolves as ") << (j.isLessEqual(i)) << '\n';
        require((j.isLessEqual(j)));
        std::cout << ("j.isLess(i) i:") << (i) << (" j:") << (j) << (" resolves as ") << (j.isLess(i)) << '\n';
        require((j.isMore(i)));
        std::cout << ("j.isMore(i) i:") << (i) << (" j:") << (j) << (" resolves as ") << (j.isMore(i)) << '\n';
        require(!(i.isMore(j)));
        std::cout << ("i.isMore(j) i:") << (i) << (" j:") << (j) << (" resolves as ") << (i.isMore(j)) << '\n';
        require(!(i.isMoreEqual(j)));
        std::cout << ("i.isMoreEqual(j) i:") << (i) << (" j:") << (j) << (" resolves as ") << (i.isMoreEqual(j)) << '\n';
        require((j.isMoreEqual(j)));
        std::cout << ("j.isMoreEqual(j) j:") << (j) << (" j:") << (j) << (" resolves as ") << (j.isMoreEqual(j)) << '\n';
    }

    static void testReadFromStreamData() {
        try {
            std::string value = "0xFFFF";
            MiniData j=MiniData::fromHex(value);
            std::vector<uint8_t> bos;
            auto& dos=bos;
            j.writeDataStream(dos);

            for (int l = 0; l < 10000; l++) {
                bos.push_back(uint8_t(32));
            }

            auto dis=bos; size_t offset=0;

            j.readDataStream(dis,offset);
            notNull(j);
            std::cout << ("j.toString = ") << (j.to0xString()) << '\n';
            require((javaStringEqualsIgnoreCase(j.to0xString(),value)));
            std::cout << (" j is now equal to ") << (j) << '\n';

            bos.clear();

            j.readDataStream(dis,offset,27);

        } catch (const wire::IOException& e) {
            std::cout << ("IOException: ") << (nativeExceptionText(e)) << (" msg=") << (std::string(e.what())) << '\n';
            require((true));
        }

    }

    static void testReadFromStreamDataTooBig() {
        try {
            MiniData j=MiniData::fromHex("0xFFFFFFFFFFFFFFFFFFFFFF" "FFFFFFFFFFFFFFFFFFFF" "FFFFFFFFFFFFFFFFFFFF" "FFFF" "FFFFFFFFFFFFFFFFFFFF" "FFFFFFFFFFFFFFFFFFFF" "FFFFFFFFFFFFFFFFFFFF" "FFFFFFFFFFFFFFFFFFFF");

            std::vector<uint8_t> bos;
            auto& dos=bos;
            j.writeDataStream(dos);
            std::cout << (" j is now equal to ") << (j) << '\n';
            for (int l = 0; l < 10000000; l++) {
                bos.push_back(uint8_t(128));
            }

            auto dis=bos; size_t offset=0;

            j.readDataStream(dis,offset,-1);
            offset=0;
            (void)MiniData::deserialise(dis,offset);

            std::cout << (" j is now equal to ") << (j) << '\n';
        } catch (const wire::IOException& e) {
            std::cout << ("IOException: ") << (nativeExceptionText(e)) << (" msg=") << (std::string(e.what())) << '\n';
            require(((std::string(e.what()).find("input too large")!=std::string::npos)));
        }

    }

    static void testWriteHashStreamData() {
        try {

            MiniData k=MiniData::fromHex("0xFFFFF");

            std::vector<uint8_t> bos;
            auto& dos=bos;
            k.writeHashToStream(dos);

            for (int l = 0; l < 1000; l++) {
                bos.push_back(uint8_t(68));
            }

            notNull(k);
        } catch (const wire::IOException& e) {
            require(((std::string(e.what()).find("input too large")!=std::string::npos)));
        }

    }

    static void testReadHashFromStream() {

        MiniData j=MiniData::fromHex("0xFFFFFFFFFFFFFFFFFFFFF" "FFFFFFFFFFFFFFFFFFFF" "FFFFFFFFFFFFFFFFFFFF" "FFFF" "FFFFFFFFFFFFFFFFFFFF" "FFFFFFFFFFFFFFFFFFFF" "FFFFFFFFFFFFFFFFFFFF" "F");
        MiniData k;

        try {
            std::vector<uint8_t> bos;
            auto& dos=bos;
            j.writeHashToStream(dos);

            auto dis=bos; size_t offset=0;

            k = MiniData::deserialiseHash(dis,offset);
            notNull(k);
        } catch (const wire::IOException& e) {
            require(!(true));
        }
    }

    static void testReadHashStreamDataLarge() {
        try {

            MiniData j=MiniData::fromHex("0xFFFFFFFFFFFFFFFFFFFFF" "FFFFFFFFFFFFFFFFFFFF" "FFFFFFFFFFFFFFFFFFFF" "FFFF" "FFFFFFFFFFFFFFFFFFFF" "FFFFFFFFFFFFFFFFFFFF" "FFFFFFFFFFFFFFFFFFFF" "F");
            MiniData k=MiniData::fromHex("0xFFFFF");

            std::vector<uint8_t> bos;
            auto& dos=bos;

            for (int l = 0; l < 1000; l++) {
                bos.push_back(uint8_t(127));
            }

            auto dis=bos; size_t offset=0;

            k = MiniData::deserialiseHash(dis,offset);
            notNull(k);

            require(!(true));
        } catch (const wire::IOException& e) {
            require(((std::string(e.what()).find(" HASH Length greater than 64")!=std::string::npos)));
        }

    }

}
int main(){std::cout<<std::boolalpha;testValueBytes();testReadFromStreamData();testReadFromStreamDataTooBig();testWriteHashStreamData();testReadHashFromStream();testReadHashStreamDataLarge();
json::JSONObject report;report.put("class","MiniDataTests");report.put("tests",6);report.put("assertions",assertions);report.put("passed",true);std::cout<<"ORIGINAL_BASE_RESULT "<<report.toJSONString()<<'\n';}
