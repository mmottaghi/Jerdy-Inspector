// This is a machine-generated code: Do not modify.
// Scripted by: Mohammad Mottaghi (mamad [a^t] cs.duke.edu)
// Generated on: Monday, 9/5/2016 5:25:16 PM

struct CJerdyParser::Nesting
{
  static const SNestingRules _nrSelf_closing[];
  static const SNestingRules _nrDefault[];
  static const SNestingRules _nrBody[];
  static const SNestingRules _nrA[];
  static const SNestingRules _nrButton[];
  static const SNestingRules _nrDefiners[];
  static const SNestingRules _nrHeadings[];
  static const SNestingRules _nrLi[];
  static const SNestingRules _nrOl[];
  static const SNestingRules _nrObject[];
  static const SNestingRules _nrTable[];
  static const SNestingRules _nrCaption[];
  static const SNestingRules _nrTbody[];
  static const SNestingRules _nrColgroup[];
  static const SNestingRules _nrCol[];
  static const SNestingRules _nrTr[];
  static const SNestingRules _nrTd[];
  static const SNestingRules _nrP[];
  static const SNestingRules _nrOption[];
  static const SNestingRules _nrOptgroup[];
  static const SNestingRules _nrHead[];
  static const SNestingRules _nrSelect[];
  static const SNestingRules _nrForm[];
 
  static const SNestabilityClass nSelf_closing[];
  static const SNestabilityClass nDefault[];
  static const SNestabilityClass nBody[];
  static const SNestabilityClass nA[];
  static const SNestabilityClass nButton[];
  static const SNestabilityClass nDefiners[];
  static const SNestabilityClass nHeadings[];
  static const SNestabilityClass nLi[];
  static const SNestabilityClass nOl[];
  static const SNestabilityClass nObject[];
  static const SNestabilityClass nTable[];
  static const SNestabilityClass nCaption[];
  static const SNestabilityClass nTbody[];
  static const SNestabilityClass nColgroup[];
  static const SNestabilityClass nCol[];
  static const SNestabilityClass nTr[];
  static const SNestabilityClass nTd[];
  static const SNestabilityClass nP[];
  static const SNestabilityClass nOption[];
  static const SNestabilityClass nOptgroup[];
  static const SNestabilityClass nHead[];
  static const SNestabilityClass nSelect[];
  static const SNestabilityClass nForm[];
};

const SNestingRules CJerdyParser::Nesting::_nrSelf_closing[] = { {} };
const SNestingRules CJerdyParser::Nesting::_nrDefault[]      = { {icTableTags, itagOption | icHeadings} };
const SNestingRules CJerdyParser::Nesting::_nrBody[]         = { {iNoTag, iNoTag, iNoTag, icTableTags | itagHead | itagHTML | itagBody} };
const SNestingRules CJerdyParser::Nesting::_nrA[]            = { {itagA | icTableTags, itagOption | icHeadings} };
const SNestingRules CJerdyParser::Nesting::_nrButton[]       = { {itagButton | icTableTags, itagOption | icHeadings | itagLi | icDefiners | itagP} };
const SNestingRules CJerdyParser::Nesting::_nrDefiners[]     = { {icDefiners | icTableTags, itagOption | icHeadings | itagLi} };
const SNestingRules CJerdyParser::Nesting::_nrHeadings[]     = { {icHeadings | icTableTags, itagOption | itagLi | icDefiners} };
const SNestingRules CJerdyParser::Nesting::_nrLi[]           = { {itagLi | icTableTags, itagOption | icHeadings | icDefiners} };
const SNestingRules CJerdyParser::Nesting::_nrOl[]           = { {icTableTags, itagOption | icHeadings | itagLi | icDefiners} };
const SNestingRules CJerdyParser::Nesting::_nrObject[]       = { {icTableTags, itagOption | icHeadings | itagLi | icDefiners | itagP | itagA | itagButton} };
const SNestingRules CJerdyParser::Nesting::_nrTable[]        = { {itagTable, icTableTags | icTableTunl, icTableTunl, iNoTag, true} };
const SNestingRules CJerdyParser::Nesting::_nrCaption[]      = { {icTableTags, itagTable} };
const SNestingRules CJerdyParser::Nesting::_nrTbody[]        = { {icTbleDirectTags, itagTr, icTableTunl} };
const SNestingRules CJerdyParser::Nesting::_nrColgroup[]     = { {icTableTunl | icTbleDirectTags | icTableCells | itagTr | icScrStyl | itagForm | itagBody | itagHead, icTableCol} };
const SNestingRules CJerdyParser::Nesting::_nrCol[]          = { {} };
const SNestingRules CJerdyParser::Nesting::_nrTr[]           = { {icTbleDirectTags | itagTr, icTableCells, icTableTunl} };
const SNestingRules CJerdyParser::Nesting::_nrTd[]           = { {icTableTags, itagTable} };
const SNestingRules CJerdyParser::Nesting::_nrP[]            = { {icCloseP | icTableTags | itagForm, itagOption}, {icCloseP | icTableTags, itagOption} };
const SNestingRules CJerdyParser::Nesting::_nrOption[]       = { {igComboTags | icTableTags, icHeadings}, {igComboTags | icTableTags, icHeadings, iNoTag, icHeadings} };
const SNestingRules CJerdyParser::Nesting::_nrOptgroup[]     = { {icTableTags, icHeadings}, {itagOptgroup} };
const SNestingRules CJerdyParser::Nesting::_nrHead[]         = { {}, {iNoTag, iNoTag, icComment} };
const SNestingRules CJerdyParser::Nesting::_nrSelect[]       = { {icCloseSelect, itagOption, iNoTag, icSelectIgnored | itagTable}, {icCloseSelect | itagTable | icTableNoCol, itagOption, iNoTag, icSelectIgnored | icTableCol} };
const SNestingRules CJerdyParser::Nesting::_nrForm[]         = { {iNoTag, itagOption | icHeadings | itagLi | icDefiners, iNoTag, itagForm}, {icTableTags, iNoTag, icTableTunl | icScrStyl, itagForm} };
 
const SNestabilityClass CJerdyParser::Nesting::nSelf_closing[] = {{_nrSelf_closing, tiNil, tiNil, tiNil}};
const SNestabilityClass CJerdyParser::Nesting::nDefault[]      = {{_nrDefault, tiNil, tiNil, tiNil}};
const SNestabilityClass CJerdyParser::Nesting::nBody[]         = {{_nrBody, tiNil, tiNil, tiNil}};
const SNestabilityClass CJerdyParser::Nesting::nA[]            = {{_nrA, tiNil, tiNil, tiNil}};
const SNestabilityClass CJerdyParser::Nesting::nButton[]       = {{_nrButton, tiNil, tiNil, tiNil}};
const SNestabilityClass CJerdyParser::Nesting::nDefiners[]     = {{_nrDefiners, tiNil, tiNil, tiNil}};
const SNestabilityClass CJerdyParser::Nesting::nHeadings[]     = {{_nrHeadings, tiNil, tiNil, tiNil}};
const SNestabilityClass CJerdyParser::Nesting::nLi[]           = {{_nrLi, tiNil, tiNil, tiNil}};
const SNestabilityClass CJerdyParser::Nesting::nOl[]           = {{_nrOl, tiNil, tiNil, tiNil}};
const SNestabilityClass CJerdyParser::Nesting::nObject[]       = {{_nrObject, tiNil, tiNil, tiNil}};
const SNestabilityClass CJerdyParser::Nesting::nTable[]        = {{_nrTable, tiNil, tiNil, tiNil}};
const SNestabilityClass CJerdyParser::Nesting::nCaption[]      = {{_nrCaption, tiNil, tiNil, tiNil}};
const SNestabilityClass CJerdyParser::Nesting::nTbody[]        = {{_nrTbody, tiNil, tiNil, tiNil}};
const SNestabilityClass CJerdyParser::Nesting::nColgroup[]     = {{_nrColgroup, tiNil, tiNil, tiNil}};
const SNestabilityClass CJerdyParser::Nesting::nCol[]          = {{_nrCol, tiTable, tiColgroup, tiNil}};
const SNestabilityClass CJerdyParser::Nesting::nTr[]           = {{_nrTr, tiTable, tiTbody, tiNil}};
const SNestabilityClass CJerdyParser::Nesting::nTd[]           = {{_nrTd, tiTable, tiTr, tiNil}};
const SNestabilityClass CJerdyParser::Nesting::nP[]            = {{_nrP, tiNil, tiNil, tiTable}};
const SNestabilityClass CJerdyParser::Nesting::nOption[]       = {{_nrOption, tiNil, tiNil, tiSelect}};
const SNestabilityClass CJerdyParser::Nesting::nOptgroup[]     = {{_nrOptgroup, tiNil, tiNil, tiSelect}};
const SNestabilityClass CJerdyParser::Nesting::nHead[]         = {{_nrHead, tiNil, tiNil, tiNil /*  </head> acts as exception, and activates the exceptional rule set */}};
const SNestabilityClass CJerdyParser::Nesting::nSelect[]       = {{_nrSelect, tiNil, tiNil, tiTable}};
const SNestabilityClass CJerdyParser::Nesting::nForm[]         = {{_nrForm, tiNil, tiNil, tiTable}};

const SjHtmlTag CJerdyParser::tag_descriptors[] = { // Do NOT reorder; MUST end with {{NULL}}
//{{name, tag ID, tag Identity}, nestability class}, 
  {{"?",          tiNil,        }, Nesting::nSelf_closing}, 
  {{">root",      tiRoot,       }, Nesting::nSelf_closing}, 
  {{">comment",   tiComment,    icComment | icVoid}, Nesting::nSelf_closing}, 
  {{"!doctype",   tiDocType,    icVoid}, Nesting::nSelf_closing}, 
  {{">custom",    tiCustomTag,  icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"html",       tiHtml,       itagHTML}, Nesting::nSelf_closing}, 
  {{"head",       tiHead,       itagHead}, Nesting::nHead}, 
  {{"body",       tiBody,       itagBody | icSelectIgnored}, Nesting::nBody}, 
  {{"caption",    tiCaption,    icTableTags | icTbleDirectTags | icTableNoCol}, Nesting::nCaption}, 
  {{"td",         tiTd,         icTableTags | icTableCells | icTableNoCol}, Nesting::nTd}, 
  {{"th",         tiTh,         icTableTags | icTableCells | icTableNoCol}, Nesting::nTd}, 
  {{"col",        tiCol,        icSelectIgnored | icVoid | icTableTags | icTableCol}, Nesting::nCol}, 
  {{"colgroup",   tiColgroup,   icSelectIgnored | icTableTags | icTbleDirectTags | icTableCol}, Nesting::nColgroup}, 
  {{"tbody",      tiTbody,      icTableTags | icTbleDirectTags | icTableNoCol}, Nesting::nTbody}, 
  {{"tfoot",      tiTfoot,      icTableTags | icTbleDirectTags | icTableNoCol}, Nesting::nTbody}, 
  {{"thead",      tiThead,      icTableTags | icTbleDirectTags | icTableNoCol}, Nesting::nTbody}, 
  {{"tr",         tiTr,         itagTr | icTableTags | icTableNoCol}, Nesting::nTr}, 
  {{"table",      tiTable,      itagTable}, Nesting::nTable}, 
  {{"select",     tiSelect,     itagSelect | icSelectIgnored | icCloseSelect | icTableTunl}, Nesting::nSelect}, 
  {{"p",          tiP,          itagP | icSelectIgnored | icTableTunl | icCloseP}, Nesting::nP}, 
  {{"dd",         tiDd,         icSelectIgnored | icTableTunl | icCloseP | icDefiners}, Nesting::nDefiners}, 
  {{"dt",         tiDt,         icSelectIgnored | icTableTunl | icCloseP | icDefiners}, Nesting::nDefiners}, 
  {{"h1",         tiH1,         icSelectIgnored | icTableTunl | icCloseP | icHeadings}, Nesting::nHeadings}, 
  {{"h2",         tiH2,         icSelectIgnored | icTableTunl | icCloseP | icHeadings}, Nesting::nHeadings}, 
  {{"h3",         tiH3,         icSelectIgnored | icTableTunl | icCloseP | icHeadings}, Nesting::nHeadings}, 
  {{"h4",         tiH4,         icSelectIgnored | icTableTunl | icCloseP | icHeadings}, Nesting::nHeadings}, 
  {{"h5",         tiH5,         icSelectIgnored | icTableTunl | icCloseP | icHeadings}, Nesting::nHeadings}, 
  {{"h6",         tiH6,         icSelectIgnored | icTableTunl | icCloseP | icHeadings}, Nesting::nHeadings}, 
  {{"li",         tiLi,         itagLi | icSelectIgnored | icTableTunl | icCloseP}, Nesting::nLi}, 
  {{"button",     tiButton,     itagButton | icSelectIgnored | icTableTunl}, Nesting::nButton}, 
  {{"a",          tiA,          itagA | icSelectIgnored | icTableTunl}, Nesting::nA}, 
  {{"option",     tiOption,     itagOption | icTableTunl}, Nesting::nOption}, 
  {{"optgroup",   tiOptgroup,   itagOptgroup | icTableTunl}, Nesting::nOptgroup}, 
  {{"object",     tiObject,     icSelectIgnored | icTableTunl}, Nesting::nObject}, 
  {{"form",       tiForm,       itagForm | icSelectIgnored}, Nesting::nForm}, 
  {{"details",    tiDetails,    icSelectIgnored | icTableTunl | icCloseP}, Nesting::nOl}, 
  {{"article",    tiArticle,    icSelectIgnored | icTableTunl | icCloseP}, Nesting::nOl}, 
  {{"aside",      tiAside,      icSelectIgnored | icTableTunl | icCloseP}, Nesting::nOl}, 
  {{"blockquote", tiBlockquote, icSelectIgnored | icTableTunl | icCloseP}, Nesting::nOl}, 
  {{"dl",         tiDl,         icSelectIgnored | icTableTunl | icCloseP}, Nesting::nOl}, 
  {{"fieldset",   tiFieldset,   icSelectIgnored | icTableTunl | icCloseP}, Nesting::nOl}, 
  {{"figcaption", tiFigcaption, icSelectIgnored | icTableTunl | icCloseP}, Nesting::nOl}, 
  {{"figure",     tiFigure,     icSelectIgnored | icTableTunl | icCloseP}, Nesting::nOl}, 
  {{"footer",     tiFooter,     icSelectIgnored | icTableTunl | icCloseP}, Nesting::nOl}, 
  {{"header",     tiHeader,     icSelectIgnored | icTableTunl | icCloseP}, Nesting::nOl}, 
  {{"hgroup",     tiHgroup,     icSelectIgnored | icTableTunl | icCloseP}, Nesting::nOl}, 
  {{"nav",        tiNav,        icSelectIgnored | icTableTunl | icCloseP}, Nesting::nOl}, 
  {{"ol",         tiOl,         icSelectIgnored | icTableTunl | icCloseP}, Nesting::nOl}, 
  {{"pre",        tiPre,        icSelectIgnored | icTableTunl | icCloseP}, Nesting::nOl}, 
  {{"section",    tiSection,    icSelectIgnored | icTableTunl | icCloseP}, Nesting::nOl}, 
  {{"summary",    tiSummary,    icSelectIgnored | icTableTunl | icCloseP}, Nesting::nOl}, 
  {{"ul",         tiUl,         icSelectIgnored | icTableTunl | icCloseP}, Nesting::nOl}, 
  {{"menu",       tiMenu,       icSelectIgnored | icTableTunl | icCloseP}, Nesting::nOl}, 
  {{"menuitem",   tiMenuitem,   icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"abbr",       tiAbbr,       icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"address",    tiAddress,    icSelectIgnored | icTableTunl | icCloseP}, Nesting::nDefault}, 
  {{"audio",      tiAudio,      icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"bdi",        tiBdi,        icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"bdo",        tiBdo,        icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"canvas",     tiCanvas,     icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"cite",       tiCite,       icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"datalist",   tiDatalist,   icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"del",        tiDel,        icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"dfn",        tiDfn,        icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"div",        tiDiv,        icSelectIgnored | icTableTunl | icCloseP}, Nesting::nDefault}, 
  {{"dialog",     tiDialog,     icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"ins",        tiIns,        icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"kbd",        tiKbd,        icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"label",      tiLabel,      icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"legend",     tiLegend,     icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"map",        tiMap,        icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"mark",       tiMark,       icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"meter",      tiMeter,      icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"output",     tiOutput,     icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"progress",   tiProgress,   icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"q",          tiQ,          icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"rp",         tiRp,         icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"rt",         tiRt,         icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"ruby",       tiRuby,       icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"samp",       tiSamp,       icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"span",       tiSpan,       icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"sub",        tiSub,        icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"sup",        tiSup,        icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"time",       tiTime,       icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"var",        tiVar,        icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"video",      tiVideo,      icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"b",          tiB,          icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"code",       tiCode,       icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"em",         tiEm,         icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"i",          tiI,          icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"s",          tiS,          icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"small",      tiSmall,      icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"strong",     tiStrong,     icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"u",          tiU,          icSelectIgnored | icTableTunl}, Nesting::nDefault}, 
  {{"area",       tiArea,       icSelectIgnored | icTableTunl | icVoid}, Nesting::nSelf_closing}, 
  {{"base",       tiBase,       icSelectIgnored | icTableTunl | icVoid | icHeadTags}, Nesting::nSelf_closing}, 
  {{"br",         tiBr,         icSelectIgnored | icTableTunl | icVoid}, Nesting::nSelf_closing}, 
  {{"command",    tiCommand,    icSelectIgnored | icTableTunl | icVoid | icHeadTags}, Nesting::nSelf_closing}, 
  {{"embed",      tiEmbed,      icSelectIgnored | icTableTunl | icVoid}, Nesting::nSelf_closing}, 
  {{"hr",         tiHr,         icSelectIgnored | icTableTunl | icVoid | icCloseP}, Nesting::nSelf_closing}, 
  {{"img",        tiImg,        icSelectIgnored | icTableTunl | icVoid}, Nesting::nSelf_closing}, 
  {{"input",      tiInput,      icCloseSelect | icTableTunl | icVoid}, Nesting::nSelf_closing}, 
  {{"keygen",     tiKeygen,     icCloseSelect | icTableTunl | icVoid}, Nesting::nSelf_closing}, 
  {{"link",       tiLink,       icSelectIgnored | icTableTunl | icVoid | icHeadTags}, Nesting::nSelf_closing}, 
  {{"meta",       tiMeta,       icSelectIgnored | icTableTunl | icVoid | icHeadTags}, Nesting::nSelf_closing}, 
  {{"param",      tiParam,      icSelectIgnored | icTableTunl | icVoid}, Nesting::nSelf_closing}, 
  {{"source",     tiSource,     icSelectIgnored | icTableTunl | icVoid}, Nesting::nSelf_closing}, 
  {{"track",      tiTrack,      icSelectIgnored | icTableTunl | icVoid}, Nesting::nSelf_closing}, 
  {{"wbr",        tiWbr,        icSelectIgnored | icTableTunl | icVoid}, Nesting::nSelf_closing}, 
  {{"iframe",     tiIframe,     icSelectIgnored | icTableTunl | icPseudoVoid}, Nesting::nSelf_closing}, 
  {{"noscript",   tiNoscript,   icSelectIgnored | icTableTunl | icPseudoVoid | icHeadTags}, Nesting::nSelf_closing}, 
  {{"script",     tiScript,     icPseudoVoid | icScrStyl | icHeadTags}, Nesting::nSelf_closing}, 
  {{"style",      tiStyle,      icSelectIgnored | icPseudoVoid | icScrStyl | icHeadTags}, Nesting::nSelf_closing}, 
  {{"textarea",   tiTextarea,   icCloseSelect | icTableTunl | icPseudoVoid}, Nesting::nSelf_closing}, 
  {{"title",      tiTitle,      icSelectIgnored | icTableTunl | icPseudoVoid | icHeadTags}, Nesting::nSelf_closing}, 
  {{NULL}},
};
