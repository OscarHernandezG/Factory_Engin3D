#include "Application.h"
#include "parson/parson.h"
#include "pcg-c-basic-0.9/pcg_basic.h"
#include <time.h>

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	sceneIntro = new ModuleSceneIntro(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	gui = new ModuleImGui(this);
	geometry = new ModuleGeometry(this);
	gameObject = new ModuleGameObject(this);
	importer = new ModuleImporter(this);
	picking = new ModulePicking(this);
	time = new ModuleTime(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);

	// Scenes
	AddModule(importer);
	AddModule(sceneIntro);

	// Managers
	AddModule(gameObject);
	AddModule(geometry);
	AddModule(gui);
	AddModule(picking);
	AddModule(time);

	// Renderer last!
	AddModule(renderer3D);
}

Application::~Application()
{
	list<Module*>::const_iterator item = list_modules.begin();

	while(item != list_modules.end())
	{
		delete (*item);
		list_modules.erase(item);
		item = list_modules.begin();
	}
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules

	InitialLoad();

	list<Module*>::const_iterator item = list_modules.begin();
	item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Init();
		item++;
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Start();
		item++;
	}
	
	pcg32_srandom(::time(NULL) ^ (intptr_t)&printf, 54u);
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	if (!renderer3D->vsync && toCap) {
		float toVsync = App->time->Getdt();
	
		if (capFrames > 0)
			toVsync = 1000 / capFrames;
	
		if (App->time->Getdt() < toVsync)
			SDL_Delay(toVsync - App->time->Getdt());
	}
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();

	list<Module*>::const_iterator item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PreUpdate();
		item++;
	}

	item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->Update();
		item++;
	}

	item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PostUpdate();
		item++;
	}


	item = list_modules.begin();
	//LOAD & SAVE
	if (canLoad)
	{
		JSON_Value *user_data = json_parse_file("user_data.json");
		if (user_data != NULL)
		{
			JSON_Object* dataObj = json_object(user_data);
			Load(dataObj);
			while (item != list_modules.end() && ret == UPDATE_CONTINUE)
			{
				ret = (*item)->Load(dataObj);
				item++;

			}
		}
		json_value_free(user_data);
		canLoad = false;
	}

	item = list_modules.begin();
	if (canSave)
	{
		JSON_Value *user_data = json_parse_file("user_data.json");
		if (user_data == NULL)
			user_data = json_value_init_object();
		JSON_Object* dataObj = json_object(user_data);
		
		Save(dataObj);
		while (item != list_modules.end() && ret == UPDATE_CONTINUE)
		{
			ret = (*item)->Save(dataObj);
			item++;
		}
		//This saves de json more readable
		//int sizeBuf = json_serialization_size_pretty(user_data);
		//char* buf = new char[sizeBuf];
		//json_serialize_to_buffer_pretty(user_data, buf, sizeBuf);
		//App->importer->SaveFile("test", sizeBuf, buf, LlibraryType::LlibraryType_MESH);
		//delete[] buf;

		json_serialize_to_file(user_data, "user_data.json");
		json_value_free(user_data);
		canSave = false;

	}
	
	FinishUpdate();
	
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	list<Module*>::const_iterator item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->CleanUp();
		item++;
	}
	return ret;
}

void Application::LogString(const char * texLog) const
{
	if(gui != nullptr)
		gui->LogConsole(texLog);
}

void Application::ChangeAppName(char * newName)
{
	aplicationName = newName;
	App->window->SetTitle(aplicationName.data());
}

update_status Application::Save(JSON_Object * object)
{
	json_object_dotset_string(object, "aplicationValues.Name", aplicationName.data());
	json_object_dotset_boolean(object, "aplicationValues.IsCapped", toCap);
	json_object_dotset_number(object, "aplicationValues.capFrames", capFrames);

	return UPDATE_CONTINUE;
}

update_status Application::Load(JSON_Object * object)
{
	aplicationName = json_object_dotget_string(object, "aplicationValues.Name");
	toCap = json_object_dotget_boolean(object, "aplicationValues.IsCapped");
	capFrames = json_object_dotget_number(object, "aplicationValues.capFrames");

	return UPDATE_CONTINUE;
}

void Application::InitialLoad()
{
	list<Module*>::const_iterator item = list_modules.begin();
	JSON_Value *user_data = json_parse_file("user_data.json");
	if (user_data != NULL)
	{
		JSON_Object* dataObj = json_object(user_data);
		Load(dataObj);
		while (item != list_modules.end())
		{
			(*item)->Load(dataObj);
			item++;
		}
	}
	json_value_free(user_data);


}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}