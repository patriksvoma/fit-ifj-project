# Parser grammar
Author: Patrik Švoma

```
<prologue>			::	import ifj26.* <func-list>
<func-list>			::	<func-annotation> <func-list>
<func-list>			::	ε
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
<statement>			::	if <expression> then <st-list> else <st-list>
<statement>			::	while <expression> do <st-list>
<statement>			::	return <expression>
<val-var-def>		::	: <type> = <expression>
<val-var-def>		::	= <expression>
<funcall-assign>	::	( <call-param-first>
<funcall-assign>	::	= <expression>
<call-param-first>	::	)
<call-param-first>	::	<call-param> <call-param-list>
<call-param-list>	::	)
<call-param-list>	::	, <call-param> <call-param-list>
<call-param>		::	<expression>
<expression>		::	TODO
```
