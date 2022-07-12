# StickyBuffer
Very naive implementation of a buffer that can be circular or not as well as protected from non-voluntary modifications. It can store different datatypes by employing basic templates.
At this point it is possible to insert and extract elements of different datatypes, but no information about them is stored. The user must therefore know what datatype was originally inserted to avoid casting values to wrong datatypes and extract garbage information. 
