bool32
IsStackEmpty(stack* Stack)
{
	return Size == 0;
}

#define StackPush(Stack, type, Data) (type*)_StackPush_(Stack, Data, sizeof(type))

void*
_StackPush_(stack* Stack, void* Data, uint64 DataSize)
{
	
}