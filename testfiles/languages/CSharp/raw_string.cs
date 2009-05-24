using System;

/*
	This file contains an @-string which demonstrates incorrect 
	escaping by the C# bundle.
*/	

class MyClass {
	private string r = @"\"";
	private string s = "something else";
}
