
#include <coopy/FormatSniffer.h>
#include <coopy/SocialCalcTextBook.h>
#include <coopy/CsvRead.h>
#include <coopy/JsWrap.h>
#include <coopy/FileIO.h>

#include "jsapi.h"

using namespace coopy::store;
using namespace coopy::format;
using namespace coopy::store::socialcalc;
using namespace coopy::js;
using namespace std;

extern std::string socialcalc_3();
extern std::string socialcalcconstants();
extern std::string socialcalctableeditor();
extern std::string socialcalcspreadsheetcontrol();
extern std::string socialcalcwrap();

class SCCSV : public CsvSheetReader {
public:
  CsvSheet *sheet;
  virtual CsvSheet *nextSheet(const char *name, bool named) {
    return sheet;
  }
};

static void jssetup(JsWrap& js) {
  string check = js.apply("(typeof socialcalcwrap_loaded!='undefined')?'loaded':'not loaded'");
  if (check!="loaded") {
    dbg_printf("Setup\n");
    js.apply(socialcalc_3());
    js.apply(socialcalcconstants());
    js.apply(socialcalctableeditor());
    js.apply(socialcalcspreadsheetcontrol());
    js.apply(socialcalcwrap());
  }
}

bool SocialCalcTextBook::reset() {
  if (jssheet) {
    JS_RemoveValueRoot(js.context(),jssheet);
    delete jssheet;
    jssheet = NULL;
  }
  return true;
}

bool SocialCalcTextBook::setup() {
  jssetup(js);
  reset();
  jssheet = new jsval;
  if (!jssheet) return false;
  jsval lval;
  JSBool ok = JS_CallFunctionName(js.context(), js.global(), 
				  "create_spreadsheet", 
				  0,&lval,jssheet);
  if (!jssheet) return false;
  JS_AddValueRoot(js.context(),jssheet);
  dbg_printf("Made a sheet!\n");
  js.send("set_active_sheet",jssheet);
  dbg_printf("Set a sheet!\n");
  return true;
}


bool SocialCalcTextBook::open(const Property& config) {
  if (!config.check("file")) return false;
  if (config.check("name")) {
    name = config.get("name").asString();
  }

  if (!setup()) return false;

  FormatSniffer f;
  if (!f.open(config.get("file").asString().c_str(),true)) return false;
  js.send("set_src",f.read());
  dbg_printf("Set src!\n");
  string result = js.apply("get_csv()");
  dbg_printf("[%s]\n", result.c_str());

  SCCSV reader;
  reader.sheet = &sheet;

  return CsvFile::read(result.c_str(),result.length(),reader,config)==0;
}


bool SocialCalcTextBook::write(const Property& config) {
  jssetup(js);
  if (jssheet) {
    js.send("set_active_sheet",jssheet);
  }
  string result = js.apply("get_full()");
  FileIO io;
  return io.openAndWrite(result,config);
}

bool SocialCalcTextBook::write(TextBook *book, const Property& config) {
  if (!book) return false;
  SocialCalcTextBook *prev = dynamic_cast<SocialCalcTextBook *>(book);
  if (prev) {
    return prev->write(config);
  }
  SocialCalcTextBook sc;
  if (book->getNames().size()>1) {
    fprintf(stderr,"* socialcalc plugin cannot currently deal with multiple sheets\n");
    return false;
  }
  string csv = book->readSheetByIndex(0).toString();
  if (!sc.setup()) return false;
  sc.js.send("load_from_csv",csv);
  return sc.write(config);
}