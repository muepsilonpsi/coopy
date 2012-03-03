#ifndef COOPY_SOCIALCALCSHEET
#define COOPY_SOCIALCALCSHEET

#include <coopy/DataSheet.h>

namespace coopy {
  namespace store {
    namespace socialcalc {
      class SocialCalcSheet;
    }
  }
}

class coopy::store::socialcalc::SocialCalcSheet : public DataSheet {
public:
  SocialCalcSheet();

  virtual ~SocialCalcSheet();

  virtual int width() const  { return w; }
  virtual int height() const { return h; }

  virtual std::string cellString(int x, int y) const;

  virtual std::string cellString(int x, int y, bool& escaped) const;

  virtual bool cellString(int x, int y, const std::string& str) {
    return cellString(x,y,str,false);
  }

  virtual bool cellString(int x, int y, const std::string& str, bool escaped);

  virtual ColumnRef moveColumn(const ColumnRef& src, const ColumnRef& base);

  virtual bool deleteColumn(const ColumnRef& column);

  virtual ColumnRef insertColumn(const ColumnRef& base);

  virtual ColumnRef insertColumn(const ColumnRef& base, const ColumnInfo& kind);

  virtual bool modifyColumn(const ColumnRef& base, const ColumnInfo& kind);

  virtual RowRef insertRow(const RowRef& base);

  virtual bool deleteRow(const RowRef& src);

  virtual bool deleteRows(const RowRef& first, const RowRef& last);

  virtual RowRef moveRow(const RowRef& src, const RowRef& base);


  virtual bool hasDimension() const {
    return false;
  }

  virtual bool forceWidth(int width) {
    w = width;
    return true;
  }

  virtual bool deleteData();

  virtual Poly<Appearance> getCellAppearance(int x, int y);

  virtual Poly<Appearance> getRowAppearance(int y);

  virtual Poly<Appearance> getColAppearance(int x);

private:
  void *implementation;
  int w, h;
};

#endif