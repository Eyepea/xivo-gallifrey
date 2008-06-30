function xivo_empty_data(data)
{
	return(data.match(/[^\n\r\t ]/) === null);
}

function xivo_empty_node(node)
{
	if(node.nodeType === 8
	|| (node.nodeType === 3
	   && xivo_empty_data(node.data) === true) === true)
	   	return(true);

	return(false);
}

function xivo_previous_node(node)
{
	while((node = node.previousSibling))
	{
		if(xivo_empty_node(node) === false)
			return(node);
	}

	return(false);
}

function xivo_next_node(node)
{
	while((node = node.nextSibling))
	{
		if(xivo_empty_node(node) === false)
			return(node);
	}

	return(false);
}

function xivo_firstchild(obj)
{
	var child = obj.firstChild;

 	while(child)
  	{
 		if(xivo_empty_node(child) === false)
			return(child);

		child = child.nextSibling;
	}

	return(false);
}

function xivo_lastchild(obj)
{
	var child = obj.lastChild;

 	while(child)
  	{
 		if(xivo_empty_node(child) === false)
			return(child);

		child = child.previousSibling;
	}

	return(false);
}
