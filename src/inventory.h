/*
(c) 2010 Perttu Ahola <celeron55@gmail.com>
*/

#ifndef INVENTORY_HEADER
#define INVENTORY_HEADER

#include <iostream>
#include <sstream>
#include <string>
#include "common_irrlicht.h"
#include "debug.h"
#include "mapblockobject.h"
// For g_materials
#include "main.h"

class InventoryItem
{
public:
	InventoryItem();
	virtual ~InventoryItem();
	
	static InventoryItem* deSerialize(std::istream &is);
	
	virtual const char* getName() const = 0;
	// Shall write the name and the parameters
	virtual void serialize(std::ostream &os) = 0;
	// Shall make an exact clone of the item
	virtual InventoryItem* clone() = 0;
	// Shall return an image to show in the GUI (or NULL)
	virtual video::ITexture * getImage() { return NULL; }
	// Shall return a text to show in the GUI
	virtual std::string getText() { return ""; }

private:
};

#define MATERIAL_ITEM_MAX_COUNT 99

class MaterialItem : public InventoryItem
{
public:
	MaterialItem(u8 material, u16 count)
	{
		m_material = material;
		m_count = count;
	}
	/*
		Implementation interface
	*/
	virtual const char* getName() const
	{
		return "MaterialItem";
	}
	virtual void serialize(std::ostream &os)
	{
		//os.imbue(std::locale("C"));
		os<<getName();
		os<<" ";
		os<<(unsigned int)m_material;
		os<<" ";
		os<<m_count;
	}
	virtual InventoryItem* clone()
	{
		return new MaterialItem(m_material, m_count);
	}
	video::ITexture * getImage()
	{
		return g_materials[m_material].getTexture(0);
	}
	std::string getText()
	{
		std::ostringstream os;
		os<<m_count;
		return os.str();
	}
	/*
		Special methods
	*/
	u8 getMaterial()
	{
		return m_material;
	}
	u16 getCount()
	{
		return m_count;
	}
	u16 freeSpace()
	{
		if(m_count > MATERIAL_ITEM_MAX_COUNT)
			return 0;
		return MATERIAL_ITEM_MAX_COUNT - m_count;
	}
	void add(u16 count)
	{
		assert(m_count + count <= MATERIAL_ITEM_MAX_COUNT);
		m_count += count;
	}
	void remove(u16 count)
	{
		assert(m_count >= count);
		m_count -= count;
	}
private:
	u8 m_material;
	u16 m_count;
};

class MapBlockObjectItem : public InventoryItem
{
public:
	/*MapBlockObjectItem(MapBlockObject *obj)
	{
		m_inventorystring = obj->getInventoryString();
	}*/
	MapBlockObjectItem(std::string inventorystring)
	{
		m_inventorystring = inventorystring;
	}
	
	/*
		Implementation interface
	*/
	virtual const char* getName() const
	{
		return "MBOItem";
	}
	virtual void serialize(std::ostream &os)
	{
		for(;;)
		{
			size_t t = m_inventorystring.find('|');
			if(t == std::string::npos)
				break;
			m_inventorystring[t] = '?';
		}
		os<<getName();
		os<<" ";
		os<<m_inventorystring;
		os<<"|";
	}
	virtual InventoryItem* clone()
	{
		return new MapBlockObjectItem(m_inventorystring);
	}

	video::ITexture * getImage();
	std::string getText();

	/*
		Special methods
	*/
	std::string getInventoryString()
	{
		return m_inventorystring;
	}

	MapBlockObject * createObject(v3f pos, f32 player_yaw, f32 player_pitch);

private:
	std::string m_inventorystring;
};

//SUGGESTION: Split into ClientInventory and ServerInventory
class Inventory
{
public:
	Inventory(u32 size);
	~Inventory();
	void clearItems();
	void serialize(std::ostream &os);
	void deSerialize(std::istream &is);

	Inventory & operator = (Inventory &other);

	u32 getSize();
	u32 getUsedSlots();
	
	InventoryItem * getItem(u32 i);
	// Returns old item (or NULL). Parameter can be NULL.
	InventoryItem * changeItem(u32 i, InventoryItem *newitem);
	void deleteItem(u32 i);
	// Adds an item to a suitable place. Returns false if failed.
	bool addItem(InventoryItem *newitem);

	void print(std::ostream &o);
	
private:
	core::array<InventoryItem*> m_items;
	u32 m_size;
};

#endif
