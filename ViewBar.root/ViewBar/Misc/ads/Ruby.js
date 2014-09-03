function Ruby ()
{
}

if (!Array.prototype.push) { 
  Array.prototype.push = function() { 
      var startLength = this.length; 
      for (var i = 0; i < arguments.length; i++) 
      this[startLength + i] = arguments[i]; 
     return this.length; 
  } 
}

$ = function ()
{
  var elements = new Array(); 

  for (var i = 0; i < arguments.length; i++) { 
    var element = arguments[i]; 
    if (typeof element == 'string') 
      element = document.getElementById(element); 

    if (arguments.length == 1) 
      return element; 

    elements.push(element); 
  } 

  return elements;
}

$F = function ()
{
	if(arguments.length == 1)
		return document.getElementById(arguments[0]).value;
	else
	{
		var elements = new Array();
		for(var i = 0;i< arguments.length; i++)
		{
			elements.push(document.getElementById(arguments[i]).value);
		}
		return elements;
	}
}