//Hash function
template <typename myuint>
myuint hash(myuint val)
{
  val *= 9;
  val ^= val << 17;
  val ^= val << 5;
  val += val << 19;
  val &= 0x7fffffff;
  val ^= val >> 3;
  val *= 9;
  val &= 0x7fffffff;
  return val;
}

//Inverted function:
template <typename myuint>
myuint inverted_hash(myuint val)
{
  val *= 954437177;
  val &= 0x7fffffff;
  val ^= val >> 3;
  val ^= val >> 6;
  val ^= val >> 12;
  val ^= val >> 24;
  val *= 2146959361;
  val ^= val << 5;
  val ^= val << 10;
  val ^= val << 20;
  val ^= val << 17;
  val &= 0x7fffffff;
  val *= 954437177;
  val &= 0x7fffffff;
  return val;
}