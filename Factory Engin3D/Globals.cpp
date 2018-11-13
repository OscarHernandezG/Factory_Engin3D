#include "Globals.h"

#include "parson/parson.h"

void SaveNumberArray(JSON_Object* parent, char* name, float* numbers, int size)
{
	JSON_Value* value = json_value_init_array();
	JSON_Array* arrray = json_value_get_array(value);

	json_object_set_value(parent, name, value);

	for (int i = 0; i < size; ++i)
	{
		json_array_append_number(arrray, numbers[i]);
	}
}