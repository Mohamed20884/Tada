procedure main is
 x : Integer;
 y : Integer;
 z : Integer;
begin
x := 2;
 call_function test2 (x in : Integer, y out : Integer);
  x := 2;


end main;

function test (x in : Integer, y out : Integer) is
b : Integer;
begin
	b := 2;
	x := x + b;
	call_function test (x in : Integer, y out : Integer);
	x := x + b;
end test;

function test2 (x in : Integer, y out : Integer) is
b : Integer;
begin
	b := 2;
	x := x + b;
	call_function test (x in : Integer, y out : Integer);
	x := x + b;
end test2;

