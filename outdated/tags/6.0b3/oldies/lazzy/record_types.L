record fields {
   int i, j;
};

record struct {
   int i, j;
   fields f;
};

struct s, a [4], f (fields);

a [3].f.i = f (s.f).f.j;

with a [1].f do
   i = j;

with s do
   with f do
      i = f.j;

