#ifndef COOPY_POLYSHEET
#define COOPY_POLYSHEET

#include <coopy/DataSheet.h>
#include <coopy/SheetSchema.h>

namespace coopy {
  namespace store {
    class PolySheet;
  }
}

class coopy::store::PolySheet : public DataSheet {
private:
  DataSheet *sheet;
  bool owned;
  SheetSchema *schema;
  bool owned_schema;
  int dh;
  bool may_sniff;
public:
  PolySheet() {
    sheet = 0/*NULL*/;
    owned = false;
    schema = 0/*NULL*/;
    owned_schema = false;
    dh = 0;
    may_sniff = true;
  }

  PolySheet(DataSheet *sheet, bool owned) : sheet(sheet), owned(owned) {
    if (sheet!=0/*NULL*/&&owned) {
      sheet->addReference();
    }
    schema = 0/*NULL*/;
    owned_schema = false;
    dh = 0;
    may_sniff = true;
  }

  PolySheet(const PolySheet& alt) {
    owned = alt.owned;
    sheet = alt.sheet;
    dh = alt.dh;
    if (sheet!=0/*NULL*/&&owned) {
      sheet->addReference();
    }
    schema = alt.schema;
    owned_schema = alt.owned_schema;
    if (schema!=0/*NULL*/&&owned_schema) {
      schema->addReference();
    }
    may_sniff = alt.may_sniff;
  }

  const PolySheet& operator=(const PolySheet& alt) {
    clear();
    owned = alt.owned;
    sheet = alt.sheet;
    dh = alt.dh;
    if (sheet!=0/*NULL*/&&owned) {
      sheet->addReference();
    }
    schema = alt.schema;
    owned_schema = alt.owned_schema;
    if (schema!=0/*NULL*/&&owned_schema) {
      schema->addReference();
    }
    may_sniff = alt.may_sniff;
    return *this;
  }

  virtual ~PolySheet() {
    clear();
  }

  void setSchema(SheetSchema *schema, bool owned) {
    clearSchema();
    this->schema = schema;
    this->owned_schema = owned;
    if (schema!=0/*NULL*/&&owned_schema) {
      schema->addReference();
    }
  }

  virtual SheetSchema *getSchema() const {
    if (schema!=0/*NULL*/) return schema;
    if (sheet==0/*NULL*/) return 0/*NULL*/;
    return sheet->getSchema();
  }

  void clear() {
    clearSheet();
    clearSchema();
    may_sniff = true;
  }

  void clearSheet() {
    if (sheet==0/*NULL*/) return;
    if (owned) {
      int ref = sheet->removeReference();
      if (ref==0) {
	delete sheet;
      }
    }
    sheet = 0/*NULL*/;
  }
  
  void clearSchema() {
    if (schema==0/*NULL*/) return;
    if (owned_schema) {
      int ref = schema->removeReference();
      if (ref==0) {
	delete schema;
      }
    }
    schema = 0/*NULL*/;
  }

  bool isValid() const {
    return sheet!=0/*NULL*/;
  }

  virtual int width() const {
    if (!sheet) return 0;
    return sheet->width();
  }

  virtual int height() const {
    if (!sheet) return 0;
    return sheet->height()-dh;
  }

  virtual std::string cellString(int x, int y) const {
    COOPY_ASSERT(sheet);
    return sheet->cellString(x,y+dh);
  }

  virtual std::string cellString(int x, int y, bool& escaped) const {
    COOPY_ASSERT(sheet);
    return sheet->cellString(x,y+dh,escaped);
  } 

  virtual bool cellString(int x, int y, const std::string& str) {
    COOPY_ASSERT(sheet);
    return sheet->cellString(x,y+dh,str);
  }

  virtual bool cellString(int x, int y, const std::string& str, bool escaped) {
    COOPY_ASSERT(sheet);
    return sheet->cellString(x,y+dh,str,escaped);
  }

  virtual SheetCell cellSummary(int x, int y) const {
    COOPY_ASSERT(sheet);
    return sheet->cellSummary(x,y+dh);
  }

  virtual bool cellSummary(int x, int y, const SheetCell& c) {
    COOPY_ASSERT(sheet);
    return sheet->cellSummary(x,y+dh,c);
  }

  std::string encode(const SheetStyle& style) const {
    COOPY_ASSERT(sheet);
    return sheet->encode(style);
  }

  //virtual SheetSchema *getSchema() {
  //return sheet->getSchema();
  //}

  virtual bool deleteColumn(const ColumnRef& column) {
    COOPY_ASSERT(sheet);
    bool ok = sheet->deleteColumn(column);
    if (!ok) return false;
    SheetSchema *s = getSchema();
    if (s) {
      if (!s->isShadow()) {
	//if (ok&&owned_schema&&schema!=0/*NULL*/) {
	s->deleteColumn(column);
      }
    }
    return ok;
  }

  virtual ColumnRef insertColumn(const ColumnRef& base) {
    COOPY_ASSERT(sheet);
    ColumnRef result = sheet->insertColumn(base);
    if (!result.isValid()) return result;
    SheetSchema *s = getSchema();
    if (s) {
      if (!s->isShadow()) {
	ColumnRef result2 = s->insertColumn(base,ColumnInfo());
	COOPY_ASSERT(result2.getIndex()==result.getIndex());
      }
    }
    return result;
  }

  virtual ColumnRef insertColumn(const ColumnRef& base, 
				 const ColumnInfo& info);

  virtual bool modifyColumn(const ColumnRef& base, 
			    const ColumnInfo& info);

  // move a column before base; if base is invalid move after all columns
  virtual ColumnRef moveColumn(const ColumnRef& src, const ColumnRef& base) {
    COOPY_ASSERT(sheet);
    ColumnRef result = sheet->moveColumn(src,base);
    if (!result.isValid()) return result;
    SheetSchema *s = getSchema();
    if (s) {
      if (!s->isShadow()) {
	ColumnRef result2 = s->moveColumn(src,base);
	COOPY_ASSERT(result2.getIndex()==result.getIndex());
      }
    }
    return result;
  }

  virtual bool deleteRow(const RowRef& src) {
    COOPY_ASSERT(sheet);
    return sheet->deleteRow((dh==0)?src:src.delta(dh));
  }

  virtual bool deleteRows(const RowRef& first, const RowRef& last) {
    COOPY_ASSERT(sheet);
    return sheet->deleteRows((dh==0)?first:first.delta(dh),
			     (dh==0)?last:last.delta(dh));
  }

  // insert a row before base; if base is invalid insert after all rows
  virtual RowRef insertRow(const RowRef& base) {
    COOPY_ASSERT(sheet);
    return fixRow(sheet->insertRow((dh==0)?base:base.delta(dh)));
  }

  // move a row before base; if base is invalid move after all rows
  virtual RowRef moveRow(const RowRef& src, const RowRef& base) {
    COOPY_ASSERT(sheet);
    return fixRow(sheet->moveRow(src,(dh==0)?base:base.delta(dh)));
  }

  virtual bool copyData(const DataSheet& src) {
    COOPY_ASSERT(sheet);
    return sheet->copyData(src);
  }

  virtual bool canWrite() { 
    COOPY_ASSERT(sheet);
    return sheet->canWrite();
  }

  virtual bool canResize() {
    COOPY_ASSERT(sheet);
    return sheet->canResize();
  }

  virtual bool resize(int w, int h) {
    COOPY_ASSERT(sheet);
    return sheet->resize(w,h+dh);
  }

  virtual Poly<SheetRow> insertRow() {
    COOPY_ASSERT(sheet);
    Poly<SheetRow> row = sheet->insertRow();
    row->setDelta(-dh);
    return row;
  }

  virtual Poly<SheetRow> insertRowOrdered(const RowRef& base) {
    COOPY_ASSERT(sheet);
    Poly<SheetRow> row = sheet->insertRowOrdered((dh==0)?base:base.delta(dh));
    row->setDelta(-dh);
    return row;
  }

  virtual bool applySchema(const SheetSchema& ss) {
    COOPY_ASSERT(sheet);
    return sheet->applySchema(ss);
  }

  virtual bool applyRowCache(const RowCache& cache, int row, SheetCell *result) {
    COOPY_ASSERT(sheet);
    return sheet->applyRowCache(cache,row,result);
  }

  virtual bool deleteData(int start = 0) {
    COOPY_ASSERT(sheet);
    return sheet->deleteData(dh+start);
  }

  virtual bool hasDimension() const {
    COOPY_ASSERT(sheet);
    return sheet->hasDimension();
  }

  virtual bool forceWidth(int width) {
    COOPY_ASSERT(sheet);
    return sheet->forceWidth(width);
  }

  virtual bool forceHeight(int height) {
    COOPY_ASSERT(sheet);
    return sheet->forceHeight(height+dh);
  }

  virtual bool hasExternalColumnNames() const {
    if (dh!=0) return true;
    COOPY_ASSERT(sheet);
    return sheet->hasExternalColumnNames();
  }


  virtual std::string getDescription() const {
    return "poly";
  }

  virtual std::vector<std::string> getNestedDescription() const {
    COOPY_ASSERT(sheet);
    std::vector<std::string> v = sheet->getNestedDescription();
    std::string name = getDescription();
    v.insert(v.begin(),name);
    return v;
  }

  virtual std::string getHash(bool cache) const {
    if (dh==0) {
      COOPY_ASSERT(sheet);
      return sheet->getHash(cache);
    }
    return DataSheet::getHash(cache);
  }


  virtual std::string getRawHash() const {
    if (dh==0) {
      COOPY_ASSERT(sheet);
      return sheet->getRawHash();
    }
    return DataSheet::getRawHash();
  }

  virtual DataSheet& tail() {
    if (dh!=0) {
      return *this;
    }
     COOPY_ASSERT(sheet);
     return sheet->tail();
  }

  virtual const DataSheet& tail_const() const {
    if (dh!=0) {
      return *this;
    }
     COOPY_ASSERT(sheet);
     return sheet->tail_const();
  }

  virtual const DataSheet& dataTail() const {
    COOPY_ASSERT(sheet);
    return sheet->dataTail();
  }

  virtual bool isSequential() const {
    COOPY_ASSERT(sheet);
    return sheet->isSequential();
  }

  virtual DataSheet *getNestedSheet(int x, int y) {
    COOPY_ASSERT(sheet);
    return sheet->getNestedSheet(x,y);
  }

  bool setRowOffset(int dh) {
    this->dh = dh;
    return true;
  }

  virtual bool hasRowOffset() const {
    if (dh==0 && sheet) {
      return sheet->hasRowOffset();
    }
    return dh!=0;
  }

  bool setRowOffset() {
    COOPY_ASSERT(sheet);
    if (dh>0) return true;
    SheetSchema *schema = getSchema();
    if (!schema) {
      schema = sheet->getSchema();
    }
    if (!schema) return true;
    if (sheet->hasExternalColumnNames()) return true;
    if (schema->headerHeight()>=0) {
      setRowOffset(schema->headerHeight()+1);
    }
    return true;
  }

  bool createHeaders() {
    if (dh>0) return false;
    if (!hasExternalColumnNames()) {
      SheetSchema *schema = getSchema();
      if (schema) {
	insertRow(RowRef(0));
	for (int i=0; i<schema->getColumnCount(); i++) {
	  cellString(i,0,schema->getColumnInfo(i).getName());
	}
	setRowOffset(1);
	schema->setHeaderHeight(1);
	return true;
      }
    }
    return false;
  }

  bool hideHeaders() {
    if (!hasExternalColumnNames()) {
      if (getSchema()==0/*NULL*/) {
	setRowOffset(1);
      } else {
	setRowOffset(getSchema()->headerHeight());
      }
    }
    return false;
  }

  virtual Poly<Appearance> getCellAppearance(int x, int y) {
    COOPY_ASSERT(sheet);
    return sheet->getCellAppearance(x,y+dh);
  }

  virtual Poly<Appearance> getRowAppearance(int y) {
    COOPY_ASSERT(sheet);
    return sheet->getRowAppearance(y+dh);
  }

  virtual Poly<Appearance> getColAppearance(int x) {
    COOPY_ASSERT(sheet);
    return sheet->getColAppearance(x);
  }

  virtual bool hasSheetName() const {
    COOPY_ASSERT(sheet);
    return sheet->hasSheetName();
  }

  virtual bool addedHeader() {
    COOPY_ASSERT(sheet);
    return sheet->addedHeader();
  }

  bool mustHaveSchema();

  void forbidSchema() {
    may_sniff = false;
  }

  virtual bool setPool(Pool *pool) {
    COOPY_ASSERT(sheet);
    return sheet->setPool(pool);
  }

  virtual Pool *getPool() const {
    COOPY_ASSERT(sheet);
    return sheet->getPool();
  }

  virtual void setMeta(SheetSchema *hint) {
    COOPY_ASSERT(sheet);
    sheet->setMeta(hint);
  }

  virtual void setMeta() {
    COOPY_ASSERT(sheet);
    SheetSchema *schema = getSchema();
    if (!schema) return;
    setMeta(schema->clone());
  }

  virtual SheetSchema *getMeta() const {
    COOPY_ASSERT(sheet);
    return sheet->getMeta();
  }

  virtual bool beginTransaction() {
    COOPY_ASSERT(sheet);
    return sheet->beginTransaction();
  }
  
  virtual bool rollbackTransaction() {
    COOPY_ASSERT(sheet);
    return sheet->rollbackTransaction();
  }
  
  virtual bool endTransaction() {
    COOPY_ASSERT(sheet);
    return sheet->endTransaction();
  }

  virtual void *getDatabase() const {
    COOPY_ASSERT(sheet);
    return sheet->getDatabase();
  }

  virtual coopy::cmp::Compare *getComparisonMethod() {
    COOPY_ASSERT(sheet);
    return sheet->getComparisonMethod();
  }

private:
  RowRef fixRow(const RowRef& r) {
    if (dh==0) return r;
    int idx = r.getIndex();
    if (idx==-1) return r;
    return r.delta(-dh);
  }

  SheetSchema *preApplyInfo(const ColumnInfo& info);
  bool postApplyInfo(const ColumnInfo& info, const ColumnRef& result);
};

#endif
