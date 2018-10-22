
class Importer
{
public:
	Importer();
	~Importer();

	virtual bool Import() {};
	virtual bool Load() {};

private:

};

Importer::Importer()
{
}

Importer::~Importer()
{
}