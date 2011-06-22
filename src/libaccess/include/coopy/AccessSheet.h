#ifndef COOPY_ACCESSSHEET
#define COOPY_ACCESSSHEET

#include <coopy/DataSheet.h>
#include <coopy/SparseSheet.h>
#include <coopy/Property.h>

#include <vector>

namespace coopy {
  namespace store {
    class AccessSheet;
    class AccessSheetSchema;
  }
}

class coopy::store::AccessSheet : public DataSheet {
public:
  AccessSheet(void *db, const char *name, const Property& config);

  bool connect();

  virtual ~AccessSheet();

  virtual int width() const  { return w; }
  virtual int height() const { return h; }

  virtual std::string cellString(int x, int y) const;

  virtual std::string cellString(int x, int y, bool& escaped) const;

  virtual ColumnInfo getColumnInfo(int x) {
    ColumnType t(col2type[x]);
    t.primaryKey = col2pk[x];
    t.primaryKeySet = true;
    //printf(">> %d %s\n", x, col2sql[x].c_str());
    return ColumnInfo(col2sql[x],t);
  }

  virtual int getColumnCount() const {
    return (int)col2sql.size();
  }

  virtual SheetSchema *getSchema() const;

  virtual bool hasExternalColumnNames() const {
    return true;
  }

  std::string getName() const {
    return name;
  }

private:
  AccessSheetSchema *schema;
  Property config;
  void *implementation;
  void *table_implementation;
  std::string name;
  int w, h;
  std::vector<std::string> col2sql;
  std::vector<std::string> col2type;
  std::vector<bool> col2pk;
  SparseStringSheet cache;
  SparseByteSheet cacheFlag;
 
};

class coopy::store::AccessSheetSchema : public SheetSchema {
public:
  AccessSheet *sheet;

  virtual std::string getSheetName() const {
    return sheet->getName();
  }

  virtual ColumnInfo getColumnInfo(int x) const {
    return sheet->getColumnInfo(x);
  }

  virtual int getColumnCount() const {
    return sheet->getColumnCount();
  }

  virtual bool providesPrimaryKeys() const {
    return true;
  }
};

#endif
