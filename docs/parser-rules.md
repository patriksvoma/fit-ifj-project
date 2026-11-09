# Parser grammar
Author: Patrik Švoma

```
<prologue>			::	import ifj26.* <func-annotation>
<func-annotation>	::	@main <func>
<func-annotation>	::	<func>
<func>				::	def id ( <param-first>
<param-first>		::	) <return-type>
<param-first>		::	<param> <param-list>
<param-list>		::	) <return-type>
<param-list>		::	, <param> <param-list>
<param>				::	id : type
<return-type>		::	= <st-list>
<return-type>		::	: type  = <st-list>
<st-list>			::	<statement> <st-list>
<st-list>			::	<func-annotation>
<statement>			::	TODO
```
