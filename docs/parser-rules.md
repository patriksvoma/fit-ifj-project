# Parser grammar
Author: Patrik Švoma

## Top-down parser rules
```
1   <prologue>			::	import ifj26 . * <func-list>
2   <func-list>			::	<func-annotation> <func-list>
3   <func-list>			::	<ext-def> <func-list>
4   <func-list>			::	EOF
5   <ext-def>			::	extension ( <param> ) <func-def> <ext-def-list>
6   <ext-def-list>		::	ε
7   <ext-def-list>		::	<func-def> <ext-def-list>
8   <func-annotation>	::	@main <func-def>
9   <func-annotation>	::	<func-def>
10  <func-def>			::	def id <func-params> <return-type>
11  <func-params>		::	ε
12  <func-params>		::	( <param-first>
13  <param-first>		::	) <func-params>
14  <param-first>		::	<param> <param-list>
15  <param-list>		::	) <func-params>
16  <param-list>		::	, <param> <param-list>
17  <param>				::	id : <type>
18  <return-type>		::	= <block-start>
19  <return-type>		::	: <type> = <block-start>
20  <block-start>		::	<statement> <block>
21  <block>				::	<statement> <block>
22  <block>				::	ε
23  <type>				::	Double
24  <type>				::	Int
25  <type>				::	String
26  <type>				::	Unit
27  <type>				::	Boolean
28  <statement>			::	val id <val-var-def>
29  <statement>			::	var id <val-var-def>
30  <statement>			::	if <expression> then <block-start> <else-opt>
31  <statement>			::	while <expression> do <block-start>
32  <statement>			::	for id <- <expression> <for-generator> <for-guard> do <block-start>
33  <statement>			::	return <expression-opt>
34  <statement>			::	id = <expression>
35  <statement>			::	<expression>
36  <expression-opt>	::	ε
37  <expression-opt>	::	<expression>
38  <else-opt>			::	ε
39  <else-opt>			::	else <else-if-opt>
40  <else-if-opt>		::	if <expression> then <block-start> <else-opt>
41  <else-if-opt>		::	<block-start>
42  <for-generator>		::	to <expression> <for-by>
43  <for-generator>		::	until <expression>
44  <for-by>			::	by <expression>
45  <for-by>			::	ε
46  <for-guard>			::	if <expression> <for-guard>
47  <for-guard>			::	ε
48  <val-var-def>		::	: <type> = <expression>
49  <val-var-def>		::	= <expression>
50  <expression>		::	SWITCH TO OPERATOR PRECEDENCE PARSER
```

## Expression parser rules
```
1   E :: i
2   E :: (E)
3   E :: E.E
4   E :: !E
5   E :: +E
6   E :: -E
7   E :: E*E
8   E :: E/E
9   E :: E+E
10  E :: E-E
11  E :: E<E
12  E :: E<=E
13  E :: E>E
14  E :: E>=
15  E :: E==E
16  E :: E!=E
17  E :: E&&E
18  E :: E||E
```
