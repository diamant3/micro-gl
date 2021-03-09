#pragma once

template <unsigned P>
class Q {
private:
    using index = unsigned int;
    using integer = long long;
    using precision_t = unsigned char;
    using const_ref = const Q &;
    using const_exact_ref = const Q<P> &;
    using const_signed_ref = const integer &;
    using q_ref = Q &;

    inline
    integer convert_q_value_to_my_space(const_ref q) {
        return convert(q.value(), q.precision, this->precision);
    }

    static
    integer abs(const integer & val) {
        return val<0?-val:val;
    }
    static integer sign(const integer & val) {
        return val<0?-1:1;
    }
public:
    static const precision_t precision = P;
    integer _value = 0;

    static inline
    integer convert(integer from_value,
                precision_t from_precision,
                precision_t to_precision) {
        if(from_precision==to_precision)
            return from_value;
        else {
            const bool isNegative=from_value<0;
            integer from_value_abs=abs(from_value);
            if(from_precision>to_precision) {
                integer inter=from_value_abs>>(from_precision - to_precision);
                return isNegative ? -inter : inter;
            } else {
                integer inter=from_value_abs<<(to_precision - from_precision);
                return isNegative ? -inter : inter;
            }
        }
    }

    Q() {
        this->_value = 0;
    }

    Q(const_ref q) {
        this->_value = q.value();
    }
    // conversion constructor, this reduces many other
    // operators.
    template <unsigned P_2>
    Q(const Q<P_2> &q) {
        this->_value = convert(q.value(),
                             P_2,
                             P);
    }
    Q(const_signed_ref q_val, precision_t q_precision) {
        this->_value = convert(q_val, q_precision, P);
    }
    // this is Q<0>, so we promote it to Q<P>
    Q(const_signed_ref int_val) {
        this->_value = int_val<<P;
    }

    Q(const signed& int_val) {
        const bool isNegative=int_val<0;
        this->_value = abs(integer(int_val))<<P;
        if(isNegative) this->_value = -this->_value;
    }
    Q(const unsigned & int_val) {
        this->_value = integer(int_val)<<P;
    }
    Q(const float &val) {
        this->_value = integer(val * float(1u<<P));
    }
    Q(const double &val) {
        this->_value = integer(val * double(1u<<P));
    }

    // with assignments operators
    q_ref operator =(const_ref q) {
        this->_value = q.value();
        return *this;
    }
    q_ref operator =(const float &float_value) {
        return (*this)=Q{float_value};
    }
    q_ref operator *=(const_ref q) {
        long long inter = ((long long)this->_value*q.value());
        const bool isNegative=inter<0;
        this->_value = abs(inter)>>P;
        if(isNegative) this->_value = -this->_value;
        return *this;
    }
    q_ref operator /=(const_ref q) {
        const bool isNegative=_value<0;
        integer value_abs=abs(_value)<<P;
        this->_value = value_abs/q.value();
        if(isNegative) this->_value = -this->_value;
        return *this;
    }
    q_ref operator +=(const_ref q) {
        this->_value+=q.value();
        return *this;
    }
    q_ref operator -=(const_ref q) {
        this->_value-=q.value();
        return *this;
    }

    // int assignments
    q_ref operator *=(const_signed_ref i) {
        this->_value*=i;
        return *this;
    }
    q_ref operator /=(const_signed_ref i) {
        this->_value/=i;
        return *this;
    }
    q_ref operator +=(const_signed_ref i) {
        this->_value+=(i<<P);
        return *this;
    }
    q_ref operator -=(const_signed_ref i) {
        this->_value -= (i<<P);
        return *this;
    }

    // intermediate Q
    Q operator +(const_ref q) const {
        Q temp{*this}; temp+=q;
        return temp;
    }
    Q operator -(const_ref q) const {
        Q temp{*this}; temp-=q;
        return temp;
    }
    Q operator *(const_ref q) const {
        Q temp{*this}; temp*=q;
        return temp;
    }
    Q operator /(const_ref q) const {
        Q temp{*this}; temp/=q;
        return temp;
    }

    // intermediate int
    Q operator +(const_signed_ref i) const {
        Q temp; temp.updateValue(this->_value+(i<<P));
        return temp;
    }
    Q operator -(const_signed_ref i) const {
        Q temp; temp.updateValue(this->_value-(i<<P));
        return temp;
    }
    Q operator *(const_signed_ref i) const {
        Q temp; temp.updateValue(this->_value*i);
        return temp;
    }
    Q operator /(const_signed_ref i) const {
        Q temp; temp.updateValue(this->_value/i);
        return temp;
    }

    // negate
    Q operator -() const { return Q{-this->value(), P}; }

    // booleans
    bool operator <(const_ref q) const { return this->_value<q._value; }
    bool operator >(const_ref q) const { return this->_value>q._value; }
    bool operator <=(const_ref q) const { return this->_value<=q._value; }
    bool operator >=(const_ref q) const { return this->_value>=q._value; }
    bool operator ==(const_ref q) const { return this->_value==q._value; }
    bool operator !=(const_ref q) const { return this->_value!=q._value; }
    Q operator %(const_ref q) const {
        Q res;
        res._value = this->value()%q.value();
        return res;
    }

    // conversion operators
    explicit operator bool() const { return this->_value!=0; }
    explicit operator float() const { return toFloat(); }
    explicit operator double() const { return toFloat(); }
    explicit operator short() const { return toInt(); }
    explicit operator unsigned short() const { return toInt(); }
    explicit operator int() const { return toInt(); }
    explicit operator unsigned int() const { return toInt(); }
    explicit operator long long() const { return toInt(); }
    explicit operator unsigned long long() const { return toInt(); }
    explicit operator char() const { return toInt(); }
    explicit operator unsigned char() const { return toInt(); }

    integer toInt() const { return this->_value>>P; }
    integer toFixed(precision_t precision_value) const { return convert(this->_value, P, precision_value); }
    float toFloat() const { return float(this->_value)/float(1u<<P); }
    integer fraction() const { return _value & ((1u<<P) - 1); }
    integer integral() const { return _value & (~((1u<<P) - 1)); } //this is wrong ?}
    inline integer value() const { return _value; }
    inline void updateValue(const_signed_ref val) { this->_value=val; }
};