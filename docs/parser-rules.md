# Parser grammar
Author: Patrik Švoma

```
<prologue>			::	import ifj26.* <func-list>
<func-list>			::	<func-annotation> <func-list>
<func-list>			::	<ext-def> <func-list>
<func-list>			::	ε
<ext-def>			::	extension ( <param> ) <func-def> <ext-def-list>
<ext-def-list>		::	ε
<ext-def-list>		::	<func-def> <ext-def-list>
<func-annotation>	::	@main <func-def>
<func-annotation>	::	<func-def>
<func-def>			::	def id ( <param-first>
<param-first>		::	) <return-type>
<param-first>		::	<param> <param-list>
<param-list>		::	) <return-type>
<param-list>		::	, <param> <param-list>
<param>				::	id : <type>
<return-type>		::	= <st-list>
<return-type>		::	: <type> = <st-list>
<st-list>			::	<statement> <st-list>
<st-list>			::	()
<type>				::	Double
<type>				::	Int
<type>				::	String
<type>				::	Unit
<type>				::	Boolean
<statement>			::	val id <val-var-def>
<statement>			::	var id <val-var-def>
<statement>			::	id <funcall-assign>
<statement>			::	if <expression> then <st-list> <else-opt>
<statement>			::	while <expression> do <st-list>
<statement>			::	for id <- <expression> <for-generator> <for-guard> do <st-list>
<statement>			::	return <expression-opt>
<expression-opt>	::	ε
<expression-opt>	::	<expression>
<else-opt>			::	ε
<else-opt>			::	else <else-if-opt>
<else-if-opt>		::	if <expression> then <st-list> <else-opt>
<else-if-opt>		::	<st-list>
<for-generator>		::	to <expression> <for-by>
<for-generator>		::	until <expression>
<for-by>			::	by <expression>
<for-by>			::	ε
<for-guard>			::	if <expression> <for-guard>
<for-guard>			::	ε
<val-var-def>		::	: <type> = <expression>
<val-var-def>		::	= <expression>
<funcall-assign>	::	( <call-param-first>
<funcall-assign>	::	= <expression>
<call-param-first>	::	)
<call-param-first>	::	<call-param> <call-param-list>
<call-param-list>	::	)
<call-param-list>	::	, <call-param> <call-param-list>
<call-param>		::	<expression>
<expression>		::	SWITCH TO OPERATOR PRECEDENCE PARSER
```
