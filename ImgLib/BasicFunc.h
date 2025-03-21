#ifndef __BASIC_FUNC_H_
#define __BASIC_FUNC_H_

__inline int Min3(int In0, int In1, int In2)
{
	if (In0 < In1)
	{
		if (In2 < In0)
		{
			return In2;
		}
		else
		{
			return In0;
		}
	}
	else
	{
		if (In2 < In1)
		{
			return In2;
		}
		else
		{
			return In1;
		}
	}
}

__inline int Max3(int In0, int In1, int In2)
{
	if (In0 < In1)
	{
		if (In2 < In1)
		{
			return In1;
		}
		else
		{
			return In2;
		}
	}
	else
	{
		if (In2 < In0)
		{
			return In0;
		}
		else
		{
			return In2;
		}
	}
}

__inline int Mid3(int In0, int In1, int In2)
{
	if (In0 < In1)
	{
		if (In2 < In0)
		{
			return In0;
		}
		else if (In1 < In2)
		{
			return In1;
		}
		else
		{
			return In2;
		}
	}
	else
	{
		if (In2 < In1)
		{
			return In1;
		}
		else if (In0 < In2)
		{
			return In0;
		}
		else
		{
			return In2;
		}
	}
}

__inline void MinMax3(int C[], int &min, int &max)
{
	if (C[0]<C[1])
	{
		min = C[0];
		max = C[1];
	}
	else
	{
		min = C[1];
		max = C[0];
	}
	if (min>C[2])min = C[2];
	if (max<C[2])max = C[2];
}

#endif

