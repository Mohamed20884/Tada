procedure Program is
 x : Integer;
 y : Integer;
 z : Integer;
begin
 x := 0;
 y := 1;
 for i in 1 .. 10 loop
 x := x + 2;
 y := y*x;
 end loop;
 if x < y then
 z := 0;
 else
 z := 1;
 end if
end Programs;
