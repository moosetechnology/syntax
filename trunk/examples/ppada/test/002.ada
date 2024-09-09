with Text_IO; use Text_IO;
procedure Enums is
type Suit is (Hearts, Diamonds, Spades, Clubs);
type Direction is (East, West, North, South, Lost);
X : Suit;
I : Integer := east + west;
begin
X := East;
X := X + West;
if X /= West then
Put_Line (Integer'Image (Integer (X)));
end if;
end Enums;

