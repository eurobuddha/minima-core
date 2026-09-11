// All active methods/inputs/assertions from the pinned original; reviewed translation.
#include "../../../../../support/OriginalBaseTestSupport.hpp"
using namespace minima;
namespace {
int assertions=0;
void require(bool value){++assertions;if(!value)throw std::runtime_error("MiniNumberTests assertion "+std::to_string(assertions));}
// Java autoboxes primitive results. Native values are materialized as owned
// objects for the same non-null smoke check; no stronger equality is substituted.
template<class T> void notNull(const T& value){auto boxed=std::make_unique<T>(value);require(bool(boxed));}

    static void testValueZero() {
        MiniNumber i(0);
        std::cout << ("i value is ") << (i) << '\n';
        notNull(i);
        require(!(i.isEqual(MiniNumber::ONE)));
        std::cout << ("i.isEqual(MiniNumber.ONE) i:") << (i) << (" resolves as ") << (i.isEqual(MiniNumber::ONE)) << '\n';
        require((i.isEqual(MiniNumber::ZERO)));
        std::cout << ("i.isEqual(MiniNumber.ZERO) i:") << (i) << (" resolves as ") << (i.isEqual(MiniNumber::ZERO)) << '\n';
        require((i.isLess(MiniNumber::ONE)));
        std::cout << ("i.isLess(MiniNumber.ONE) i:") << (i) << (" resolves as ") << (i.isLess(MiniNumber::ONE)) << '\n';
        require((i.add(MiniNumber::ONE).isEqual(MiniNumber::ONE)));
        std::cout << ("i.add(MiniNumber.ONE).isEqual(MiniNumber.ONE) i:") << (i) << (" resolves as ") << (i.add(MiniNumber::ONE).isEqual(MiniNumber::ONE)) << '\n';
        require((i.increment().isEqual(MiniNumber::ONE)));
        std::cout << ("i.increment().isEqual(MiniNumber.ONE) i:") << (i) << (" resolves as ") << (i.increment().isEqual(MiniNumber::ONE)) << '\n';
        require((i.mult(MiniNumber::ONE).isEqual(MiniNumber::ZERO)));
        std::cout << ("i.mult(MiniNumber.ONE).isEqual(MiniNumber.ZERO)) i:") << (i) << (" resolves as ") << (i.mult(MiniNumber::ONE).isEqual(MiniNumber::ZERO)) << '\n';
        require(((i.toString()=="0")));
        std::cout << ("i.toString().contentEquals(\"0\") i:") << (i) << (" resolves as ") << ((i.toString()=="0")) << '\n';

    }

    static void testValues() {
        MiniNumber i;
        MiniNumber j(int64_t(2147483649LL));
        BigInt k("2147483649");
        MiniNumber l(k);
        MMREntryNumber m(BigInt(1),0); // BigDecimal(double 1.0) has coefficient 1, scale 0.
        std::cout << ("MiniNumber() i value is ") << (i) << '\n';
        std::cout << ("MiniNumber(2147483649L) j value is ") << (j) << '\n';
        std::cout << ("BigInteger(\"2147483649\") k value is ") << (k) << '\n';
        std::cout << ("MiniNumber(k) l value is ") << (l) << '\n';
        std::cout << ("BigDecimal(1.00) m value is ") << (m) << '\n';
        notNull(i);
        notNull(j);
        notNull(k);
        notNull(l);
        notNull(MiniNumber::ONE);
        std::cout << ("MiniNumber.ONE value is ") << (MiniNumber::ONE) << '\n';
        notNull(MiniNumber::ONE.getAsBigInteger());
        std::cout << ("MiniNumber.ONE.getAsBigInteger() value is ") << (MiniNumber::ONE.getAsBigInteger()) << '\n';
        notNull(MiniNumber::ONE.getAsInt());
        std::cout << ("MiniNumber.ONE.getAsInt() value is ") << (MiniNumber::ONE.getAsInt()) << '\n';
        notNull(MiniNumber::ONE.abs());
        std::cout << ("MiniNumber.ONE.abs() value is ") << (MiniNumber::ONE.abs()) << '\n';
        notNull(MiniNumber::ONE.floor());
        std::cout << ("MiniNumber.ONE.floor() value is ") << (MiniNumber::ONE.floor()) << '\n';
        notNull(MiniNumber::ONE.ceil());
        std::cout << ("MiniNumber.ONE.ceil() value is ") << (MiniNumber::ONE.ceil()) << '\n';
        notNull(i.add(MiniNumber::ONE).getAsLong());
        std::cout << ("i.add(MiniNumber.ONE).getAsLong() value is ") << (i.add(MiniNumber::ONE).getAsLong()) << '\n';

        notNull(i.increment().setSignificantDigits(19));
        std::cout << ("i.increment().setSignificantDigits(19) value is ") << (i.increment().setSignificantDigits(19)) << '\n';
        require((i.sub(MiniNumber::ONE).isEqual(MiniNumber::MINUSONE)));
        std::cout << ("i.sub(MiniNumber.ONE).isEqual(MiniNumber.MINUSONE) i:") << (i) << (" resolves as ") << (i.sub(MiniNumber::ONE).isEqual(MiniNumber::MINUSONE)) << '\n';
        require((i.pow(1).isEqual(MiniNumber::ZERO)));
        std::cout << ("i.pow(1).isEqual(MiniNumber.ZERO i:") << (i) << (" resolves as ") << (i.pow(1).isEqual(MiniNumber::ZERO)) << '\n';
        require((i.add(MiniNumber::ONE).isLess(MiniNumber::TWO)));
        std::cout << ("i.add(MiniNumber.ONE).isLess(MiniNumber.TWO) i:") << (i) << (" resolves as ") << (i.add(MiniNumber::ONE).isLess(MiniNumber::TWO)) << '\n';
        require((i.add(MiniNumber::TWO).isLessEqual(MiniNumber::TWO)));
        std::cout << ("i.add(MiniNumber.TWO).isLessEqual(MiniNumber.TWO) i:") << (i) << (" resolves as ") << (i.add(MiniNumber::TWO).isLessEqual(MiniNumber::TWO)) << '\n';
        require(!(i.add(MiniNumber::TWO).isLessEqual(MiniNumber::ONE)));
        std::cout << ("i.add(MiniNumber.TWO).isLessEqual(MiniNumber.ONE) i:") << (i) << (" resolves as ") << (i.add(MiniNumber::TWO).isLessEqual(MiniNumber::ONE)) << '\n';
        require((i.add(MiniNumber::TWO).isMore(MiniNumber::ONE)));
        std::cout << ("i.add(MiniNumber.TWO).isMore(MiniNumber.ONE) i:") << (i) << (" resolves as ") << (i.add(MiniNumber::TWO).isMore(MiniNumber::ONE)) << '\n';
        require((i.add(MiniNumber::ONE).div(MiniNumber::ONE).isEqual(MiniNumber::ONE)));
        std::cout << ("i.add(MiniNumber.ONE).div(MiniNumber.ONE).isEqual(MiniNumber.ONE) i:") << (i) << (" resolves as ") << (i.add(MiniNumber::ONE).div(MiniNumber::ONE).isEqual(MiniNumber::ONE)) << '\n';
        require((i.add(MiniNumber::ONE).modulo(MiniNumber::ONE).isEqual(MiniNumber::ZERO)));
        std::cout << ("i.add(MiniNumber.ONE).modulo(MiniNumber.ONE).isEqual(MiniNumber.ZERO) i:") << (i) << (" resolves as ") << (i.add(MiniNumber::ONE).modulo(MiniNumber::ONE).isEqual(MiniNumber::ZERO)) << '\n';
        require((i.increment().isEqual(MiniNumber::ONE)));
        std::cout << ("i.increment().isEqual(MiniNumber.ONE i:") << (i) << (" resolves as ") << (i.increment().isEqual(MiniNumber::ONE)) << '\n';
        require((i.decrement().isEqual(MiniNumber::MINUSONE)));
        std::cout << ("i.decrement().isEqual(MiniNumber.MINUSONE) i:") << (i) << (" resolves as ") << (i.decrement().isEqual(MiniNumber::MINUSONE)) << '\n';
        require(!(i.increment().setSignificantDigits(2).isEqual(MiniNumber::TWO)));
        std::cout << ("i.increment().setSignificantDigits(2).isEqual(MiniNumber.TWO) i:") << (i) << (" resolves as ") << (i.increment().setSignificantDigits(2).isEqual(MiniNumber::TWO)) << '\n';
        require(!(i.increment().isLess(MiniNumber::ONE)));
        std::cout << ("i.increment().isLess(MiniNumber.ONE) i:") << (i) << (" resolves as ") << (i.increment().isLess(MiniNumber::ONE)) << '\n';
        require(!(i.increment().isMore(MiniNumber::ONE)));
        std::cout << ("i.increment().isMore(MiniNumber.ONE) i:") << (i) << (" resolves as ") << (i.increment().isMore(MiniNumber::ONE)) << '\n';
        require(!(i.isMoreEqual(MiniNumber::ONE)));
        std::cout << ("i.isMoreEqual(MiniNumber.ONE) i:") << (i) << (" resolves as ") << (i.isMoreEqual(MiniNumber::ONE)) << '\n';
        require((i.isMoreEqual(MiniNumber::ZERO)));
        std::cout << ("i.isMoreEqual(MiniNumber.ZERO i:") << (i) << (" resolves as ") << (i.isMoreEqual(MiniNumber::ZERO)) << '\n';
        require((decimalEquals(MiniNumber::ONE.getAsBigDecimal(),m)));
        std::cout << ("MiniNumber.ONE.getAsBigDecimal().equals(m) m:") << (m) << (" value is ") << (decimalEquals(MiniNumber::ONE.getAsBigDecimal(),m)) << '\n';
    }

    static void testWriteDataStream() {
        try {
            MiniNumber i("1");
            std::vector<uint8_t> bos;
            auto& dos=bos;
            i.writeDataStream(dos);
            notNull(i);
            std::cout << (" i is now equal to ") << (i) << '\n';
        } catch (const wire::IOException& e) {
            std::cout << ("IOException: ") << (nativeExceptionText(e)) << (" msg=") << (std::string(e.what())) << '\n';
            require(((std::string(e.what()).find("input too large")!=std::string::npos)));
        }
    }

    static void testReadDataStream() {
        try {
            MiniNumber i("1");
            std::vector<uint8_t> bos;
            auto& dos=bos;
            MiniNumber::ONE.writeDataStream(dos);

            auto dis=bos; size_t offset=0;

            i.readDataStream(dis,offset);
            notNull(i);
            require((i.isEqual(MiniNumber::ONE)));
        } catch (const wire::IOException& e) {
            std::cout << ("IOException: ") << (nativeExceptionText(e)) << (" msg=") << (std::string(e.what())) << '\n';
            require((false));
        }

    }

    static void testReadStreamTooLarge() {
        try {
            MiniNumber i;
            std::vector<uint8_t> bos;
            auto& dos=bos;

            for (int j = 0; j < 1000; j++) {
                bos.push_back(uint8_t(127));
            }

            auto dis=bos; size_t offset=0;

            i.readDataStream(dis,offset);
            std::cout << (" i is now equal to ") << (i.toString()) << '\n';
            require(!(true));
        } catch (const wire::IOException& e) {
            std::cout << ("IOException: ") << (nativeExceptionText(e)) << (" msg=") << (std::string(e.what())) << '\n';
            require(((std::string(e.what()).find("input too large")!=std::string::npos)));
        }

    }

    static void testReadFromStream() {
        try {
            MiniNumber i("1");
            std::vector<uint8_t> bos;
            auto& dos=bos;
            MiniNumber::THOUSAND.writeDataStream(dos);

            auto dis=bos; size_t offset=0;

            (void)MiniNumber::deserialise(dis,offset);
            notNull(i);
            std::cout << ("i.ReadFromStream(dis) i is now equal to ") << (i) << '\n';
        } catch (const wire::IOException& e) {
            std::cout << ("IOException: ") << (nativeExceptionText(e)) << (" msg=") << (std::string(e.what())) << '\n';
            require(((std::string(e.what()).find("input too large")!=std::string::npos)));
        }

    }

}
int main(){std::cout<<std::boolalpha;testValueZero();testValues();testWriteDataStream();testReadDataStream();testReadStreamTooLarge();testReadFromStream();
json::JSONObject report;report.put("class","MiniNumberTests");report.put("tests",6);report.put("assertions",assertions);report.put("passed",true);std::cout<<"ORIGINAL_BASE_RESULT "<<report.toJSONString()<<'\n';}
