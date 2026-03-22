// ============================================================
//  reSTEM — Quest-based Learning Planner
//  C++17 / Qt6  |  Linux + macOS
//
//  Build (Linux Fedora):
//    sudo dnf install qt6-qtbase-devel cmake gcc-c++
//    cmake -B build -DCMAKE_BUILD_TYPE=Release
//    cmake --build build -j$(nproc)
//    ./build/reSTEM
//
//  Build (macOS Homebrew):
//    brew install qt6 cmake
//    cmake -B build -DCMAKE_PREFIX_PATH=$(brew --prefix qt6)
//    cmake --build build
//    open build/reSTEM.app
// ============================================================

#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QFrame>
#include <QCheckBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QDateEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QAbstractItemView>
#include <QProgressBar>
#include <QSlider>
#include <QSplitter>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QLinearGradient>
#include <QPainterPath>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDate>
#include <QCursor>
#include <QToolTip>
#include <QTimer>
#include <QScrollBar>
#include <QMouseEvent>
#include <QSizePolicy>
#include <QScreen>
#include <QRegularExpression>
#include <QUuid>
#include <QSvgRenderer>  // add this with your other includes
#include <algorithm>
#include <cmath>
#include <functional>
#include <optional>
#include <vector>


// ── Icon helper ────────────────────────────────────────────────
// Returns a QLabel showing an SVG icon at the given size and colour.
// name: filename without extension, e.g. "bolt"
// size: square pixel size
// color: tint colour (applies colorize effect) — pass "" for original colours
// Render an SVG from Qt resources to a QPixmap, optionally tinted.
QPixmap makeSvgPixmap(const QString& name, int size,
                      const QColor& color = QColor()) {
    QSvgRenderer renderer(QString(":/icons/%1.svg").arg(name));
    QPixmap px(size, size);
    px.fill(Qt::transparent);
    QPainter painter(&px);
    renderer.render(&painter);
    painter.end();
    if (color.isValid()) {
        QPainter tinter(&px);
        tinter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        tinter.fillRect(px.rect(), color);
    }
    return px;
}

// Returns a QLabel showing an SVG icon — use in layouts.
QLabel* makeIcon(const QString& name, int size,
                 const QColor& color = QColor()) {
    auto* lbl = new QLabel;
    lbl->setPixmap(makeSvgPixmap(name, size, color));
    lbl->setFixedSize(size, size);
    lbl->setAlignment(Qt::AlignCenter);
    lbl->setStyleSheet("background: transparent; border: none;");
    return lbl;
}

// Returns a QIcon from an SVG — use for tab icons and toolbar buttons.
QIcon svgIcon(const QString& name, int size, const QColor& color = QColor()) {
    return QIcon(makeSvgPixmap(name, size, color));
}

// Map from emoji character(s) to SVG name.
// Used by StatTile to swap emoji for proper SVG icons.
static const QMap<QString,QString> kEmojiToSvg = {
    {"⚡", "xp"},   {"✅", "check"},  {"🔥", "streak"},
    {"🏆", "rank"}, {"⏸",  "pause"},  {"▶",  "play"},
    {"📜", "list"}, {"📊", "stats"},   {"⚙",  "gears"},
    {"⚙️","gears"}, {"📦", "archive"}, {"🎯", "rank"},
    {"📈", "stats"},{"📋", "list"},    {"📅", "streak"},
    {"⬛", "check"},{"🔒", "pause"},
};

// ============================================================
//  COLOUR PALETTE  —  Khan Academy Design System
//  Primary: Teal #14BF96  ·  Navy #0A2A66  ·  Blue #1865F2
// ============================================================
namespace Cl {
    // ── Page & surface backgrounds ──────────────────────────
    constexpr auto pageBg      = "#F7F8FA";   // KA light grey page bg
    constexpr auto cardBg      = "#FFFFFF";   // pure white cards
    constexpr auto cardBg2     = "#F2F4F8";   // slightly tinted card
    constexpr auto cardBg3     = "#E8EDF5";   // deeper tint
    constexpr auto white_      = "#FFFFFF";

    // ── Borders ──────────────────────────────────────────────
    constexpr auto border      = "#D6D8DA";   // KA standard border
    constexpr auto border2     = "#BBBFC4";   // stronger border

    // ── Ink / text ────────────────────────────────────────────
    constexpr auto ink         = "#21242C";   // KA near-black
    constexpr auto ink2        = "#3D4049";
    constexpr auto ink3        = "#626569";   // secondary text
    constexpr auto ink4        = "#8D9096";   // muted / placeholder

    // ── Teal — KA signature (progress, success, completion) ──
    constexpr auto teal        = "#14BF96";   // KA primary teal
    constexpr auto teal2       = "#0D9E7C";   // hover / darker
    constexpr auto tealBg      = "#D8F7EF";   // teal tinted surface
    constexpr auto tealBdr     = "#8EE5CE";   // teal border

    // ── Blue — KA interactive / primary buttons ───────────────
    constexpr auto blue        = "#1865F2";   // KA blue
    constexpr auto blue2       = "#1254CC";   // hover / darker
    constexpr auto blueBg      = "#EAF0FE";   // blue tinted surface
    constexpr auto blueBdr     = "#9DBAF9";   // blue border

    // ── Navy — KA brand dark / headings ───────────────────────
    constexpr auto navy        = "#0A2A66";   // KA dark navy
    constexpr auto navy2       = "#1B3A82";

    // ── Purple — mastery, achievements, XP ───────────────────
    constexpr auto purple      = "#7B439E";
    constexpr auto purple2     = "#9B63BE";
    constexpr auto purpleBg    = "#F0E6F9";
    constexpr auto purpleBdr   = "#D4B5EC";

    // ── Gold / Orange — milestones, streaks, rewards ─────────
    constexpr auto gold        = "#D97706";   // amber
    constexpr auto gold2       = "#F59E0B";
    constexpr auto goldBg      = "#FEF3C7";
    constexpr auto goldBdr     = "#FCD34D";

    // ── Semantic ──────────────────────────────────────────────
    constexpr auto red         = "#D92916";   // KA red
    constexpr auto redBg       = "#FDECEA";
    constexpr auto cyan        = "#0095A3";

    // ── Aliases for backward compat (mapped to KA equivalents)
    constexpr auto cream       = pageBg;
    constexpr auto cream2      = cardBg2;   // alias
    constexpr auto cream3      = cardBg3;   // alias
    constexpr auto cream4      = "#DDE1EA";
    constexpr auto green       = teal;
    constexpr auto green2      = teal2;
    constexpr auto greenBg     = tealBg;
    constexpr auto greenBdr    = tealBdr;
    constexpr auto greenDark   = "#065F46";
    constexpr auto purpleAccent= blue;      // primary interactive → KA blue
}

// ============================================================
//  STYLESHEET  —  Khan Academy Design Language
// ============================================================
static QString buildQSS() {
    return QString(R"(
/* ── Root ── */
QMainWindow, QDialog { background: %1; }
QWidget { background: %1; color: %2; font-size: 13px; }

/* ── Tab bar ── */
QTabWidget::pane {
    border: none;
    border-top: 2px solid %3;
    background: %1;
}
QTabBar {
    background: %4;
}
QTabBar::tab {
    background: %4;
    color: %5;
    padding: 12px 28px;
    font-size: 11px;
    font-weight: 700;
    letter-spacing: 1.5px;
    border: none;
    border-bottom: 3px solid transparent;
    margin-right: 2px;
    min-width: 90px;
}
QTabBar::tab:selected {
    background: %4;
    color: %6;
    border-bottom: 3px solid %6;
}
QTabBar::tab:hover:!selected {
    color: %7;
    background: %8;
}

/* ── Scroll bars (thin, minimal) ── */
QScrollArea { border: none; background: transparent; }
QScrollBar:vertical {
    width: 6px; background: transparent; border-radius: 3px; margin: 0;
}
QScrollBar::handle:vertical {
    background: %9; border-radius: 3px; min-height: 24px;
}
QScrollBar::handle:vertical:hover { background: %5; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QScrollBar:horizontal {
    height: 6px; background: transparent; border-radius: 3px;
}
QScrollBar::handle:horizontal {
    background: %9; border-radius: 3px; min-width: 24px;
}
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }

/* ── Buttons ── */
QPushButton {
    background: %10;
    color: %5;
    border: 1.5px solid %3;
    border-radius: 7px;
    padding: 8px 16px;
    font-size: 12px;
    font-weight: 600;
    min-width: 64px;
}
QPushButton:hover {
    background: %11;
    border-color: %7;
    color: %7;
}
QPushButton:pressed { background: %9; }
QPushButton:disabled { color: %9; border-color: %3; background: %1; }

/* KA Blue — primary action */
QPushButton[role="primary"] {
    background: %6;
    color: #FFFFFF;
    border: none;
    border-radius: 7px;
    font-weight: 700;
}
QPushButton[role="primary"]:hover  { background: %7; }
QPushButton[role="primary"]:pressed { background: %6; }

/* KA Teal — success / completion */
QPushButton[role="success"] {
    background: %12;
    color: #FFFFFF;
    border: none;
    border-radius: 7px;
    font-weight: 700;
}
QPushButton[role="success"]:hover  { background: %13; }

/* Danger */
QPushButton[role="danger"] {
    background: %10;
    color: %14;
    border: 1.5px solid rgba(217,41,22,0.3);
}
QPushButton[role="danger"]:hover {
    background: %15;
    border-color: %14;
}

/* Gold / milestone */
QPushButton[role="gold"] {
    background: %16;
    color: #FFFFFF;
    border: none;
    font-weight: 700;
}
QPushButton[role="gold"]:hover { background: %17; }

/* ── Input fields ── */
QLineEdit, QTextEdit, QPlainTextEdit {
    background: %10;
    border: 1.5px solid %3;
    border-radius: 7px;
    padding: 8px 12px;
    color: %2;
    selection-background-color: %18;
    selection-color: %2;
}
QLineEdit:focus, QTextEdit:focus {
    border-color: %6;
    background: #FFFFFF;
}

QSpinBox, QDoubleSpinBox, QComboBox, QDateEdit {
    background: %10;
    border: 1.5px solid %3;
    border-radius: 7px;
    padding: 7px 10px;
    color: %2;
    min-height: 22px;
}
QSpinBox:focus, QDoubleSpinBox:focus,
QComboBox:focus, QDateEdit:focus {
    border-color: %6;
    background: #FFFFFF;
}
QComboBox::drop-down { border: none; width: 22px; }
QComboBox QAbstractItemView {
    background: #FFFFFF;
    border: 1.5px solid %3;
    border-radius: 7px;
    selection-background-color: %18;
    selection-color: %2;
    outline: none;
}

/* ── Table ── */
QTableWidget {
    background: #FFFFFF;
    border: 1.5px solid %3;
    border-radius: 10px;
    gridline-color: %3;
    outline: none;
    selection-background-color: %18;
    selection-color: %2;
    alternate-background-color: %11;
}
QTableWidget::item { padding: 5px 8px; border: none; }
QTableWidget QLineEdit { padding: 2px 10px; background: #FFFFFF; border: 1.5px solid #1865F2; border-radius: 3px; color: #21242C; font-size: 12px; }
QTableWidget::item:selected { background: %18; color: %2; }
QHeaderView {
    background: %11;
}
QHeaderView::section {
    background: %11;
    color: %5;
    padding: 9px 8px;
    border: none;
    border-right: 1px solid %3;
    border-bottom: 2px solid %3;
    font-size: 10px;
    font-weight: 700;
    letter-spacing: 2px;
    text-transform: uppercase;
}

/* ── GroupBox ── */
QGroupBox {
    border: 1.5px solid %3;
    border-radius: 12px;
    margin-top: 20px;
    padding: 18px 16px 14px;
    background: #FFFFFF;
}
QGroupBox::title {
    color: %5;
    font-size: 10px;
    font-weight: 700;
    letter-spacing: 3px;
    padding: 0 10px;
    subcontrol-origin: margin;
    subcontrol-position: top left;
    left: 16px;
    top: 4px;
    text-transform: uppercase;
}

/* ── Slider ── */
QSlider::groove:horizontal {
    height: 5px; background: %9; border-radius: 3px;
}
QSlider::handle:horizontal {
    background: %6; border: 2px solid %7;
    width: 16px; height: 16px;
    margin: -6px 0; border-radius: 8px;
}
QSlider::sub-page:horizontal { background: %6; border-radius: 3px; }

/* ── Checkbox ── */
QCheckBox { background: transparent; spacing: 8px; }
QCheckBox::indicator {
    width: 16px; height: 16px;
    border: 1.5px solid %9;
    border-radius: 4px;
    background: %10;
}
QCheckBox::indicator:checked {
    background: %12; border-color: %12;
}
QCheckBox::indicator:hover { border-color: %6; }

/* ── Progress bar ── */
QProgressBar {
    background: %9; border: none; border-radius: 3px; text-align: center;
}
QProgressBar::chunk { background: %12; border-radius: 3px; }

/* ── Tooltip ── */
QToolTip {
    background: %19;
    color: #FFFFFF;
    border: none;
    border-radius: 6px;
    padding: 6px 10px;
    font-size: 12px;
    font-weight: 500;
}

/* ── Splitter / message ── */
QSplitter::handle:horizontal { width: 1px; background: %3; }
QSplitter::handle:vertical   { height: 1px; background: %3; }
QMessageBox { background: #FFFFFF; color: #21242C; }
QMessageBox QLabel { color: #21242C; background: #FFFFFF; font-size: 13px; }
QMessageBox QPushButton { min-width: 80px; }
QDialog { background: #F7F8FA; color: #21242C; }
QDialog QLabel { color: #21242C; }
QDialog QLineEdit { color: #21242C; background: #FFFFFF; }
QInputDialog { background: #F7F8FA; color: #21242C; }
QInputDialog QLabel { color: #21242C; }
QMessageBox QPushButton { min-width: 88px; }
    )")
    // %1  page bg          %2  ink (text)       %3  border
    .arg(Cl::pageBg,        Cl::ink,              Cl::border)
    // %4  tab bar bg       %5  muted text        %6  KA blue (primary)
    .arg(Cl::cardBg,        Cl::ink3,             Cl::blue)
    // %7  KA blue2 hover   %8  blue tinted hover %9  border2 (scroll handles)
    .arg(Cl::blue2,         Cl::blueBg,           Cl::border2)
    // %10 card/input bg    %11 tinted bg         %12 KA teal (success)
    .arg(Cl::white_,        Cl::cardBg2,          Cl::teal)
    // %13 teal hover       %14 red (danger)      %15 red bg
    .arg(Cl::teal2,         Cl::red,              Cl::redBg)
    // %16 gold             %17 gold hover        %18 selection (blue tint)
    .arg(Cl::gold,          Cl::gold2,            Cl::blueBg)
    // %19 tooltip navy
    .arg(Cl::navy);
}

// ============================================================
//  DATA STRUCTURES
// ============================================================
struct Quest {
    int     day          = 1;
    QString title;
    QString description;
    int     xp           = 100;
    bool    isMilestone  = false;
    QString milestoneTitle;

    QJsonObject toJson() const {
        QJsonObject o;
        o["day"]            = day;
        o["title"]          = title;
        o["description"]    = description;
        o["xp"]             = xp;
        o["is_milestone"]   = isMilestone;
        o["milestone_title"]= milestoneTitle;
        return o;
    }
    static Quest fromJson(const QJsonObject& o) {
        Quest q;
        q.day           = o["day"].toInt(1);
        q.title         = o["title"].toString();
        q.description   = o["description"].toString();
        q.xp            = o["xp"].toInt(100);
        q.isMilestone   = o["is_milestone"].toBool(false);
        q.milestoneTitle= o["milestone_title"].toString();
        return q;
    }
};

struct RankThresholds {
    int E = 0, D = 1000, C = 3000, B = 7000, A = 15000, S = 30000;

    int forRank(const QString& r) const {
        if (r=="E") return E; if (r=="D") return D; if (r=="C") return C;
        if (r=="B") return B; if (r=="A") return A; if (r=="S") return S;
        return 0;
    }
    void setRank(const QString& r, int v) {
        if (r=="E") E=v; else if (r=="D") D=v; else if (r=="C") C=v;
        else if (r=="B") B=v; else if (r=="A") A=v; else if (r=="S") S=v;
    }
    QJsonObject toJson() const {
        QJsonObject o;
        o["E"]=E; o["D"]=D; o["C"]=C; o["B"]=B; o["A"]=A; o["S"]=S;
        return o;
    }
    static RankThresholds fromJson(const QJsonObject& o) {
        RankThresholds t;
        if (!o.isEmpty()) {
            t.E = o["E"].toInt(0);    t.D = o["D"].toInt(1000);
            t.C = o["C"].toInt(3000); t.B = o["B"].toInt(7000);
            t.A = o["A"].toInt(15000);t.S = o["S"].toInt(30000);
        }
        return t;
    }
};

struct Plan {
    QString           id;
    QString           name;
    QString           description;
    QString           startDate;    // "yyyy-MM-dd"
    int               duration = 120;
    bool              archived = false;
    bool              thresholdsManuallySet = false; // true once user edits ranks manually
    QVector<Quest>    quests;
    RankThresholds    rankThresholds;

    Quest* questForDay(int day) {
        for (auto& q : quests) if (q.day == day) return &q;
        return nullptr;
    }
    const Quest* questForDay(int day) const {
        for (const auto& q : quests) if (q.day == day) return &q;
        return nullptr;
    }
    void ensureQuest(int day) {
        if (!questForDay(day)) {
            Quest q; q.day = day;
            quests.append(q);
            std::sort(quests.begin(), quests.end(),
                      [](const Quest& a, const Quest& b){ return a.day < b.day; });
        }
    }

    bool isComplete(int completedDayCount) const {
        return duration > 0 && completedDayCount >= duration;
    }
    // Count valid completed days (within duration bounds)
    static int validCompletedCount(const QSet<int>& completedDays, int duration) {
        int count = 0;
        for (int d : completedDays) if (d <= duration) count++;
        return count;
    }
    bool isCompleteWithProgress(const QSet<int>& completedDays) const {
        return duration > 0 && validCompletedCount(completedDays, duration) >= duration;
    }

    QJsonObject toJson() const {
        QJsonObject o;
        o["id"]          = id;
        o["name"]        = name;
        o["description"] = description;
        o["start_date"]  = startDate;
        o["duration"]    = duration;
        o["archived"]    = archived;
        o["thresholds_manual"] = thresholdsManuallySet;
        QJsonArray qa;
        for (const auto& q : quests) qa.append(q.toJson());
        o["quests"]          = qa;
        o["rank_thresholds"] = rankThresholds.toJson();
        return o;
    }
    static Plan fromJson(const QJsonObject& o) {
        Plan p;
        p.id          = o["id"].toString();
        p.name        = o["name"].toString();
        p.description = o["description"].toString();
        p.startDate   = o["start_date"].toString();
        p.duration    = o["duration"].toInt(120);
        p.archived    = o["archived"].toBool(false);
        p.thresholdsManuallySet = o["thresholds_manual"].toBool(false);
        for (const auto& v : o["quests"].toArray())
            p.quests.append(Quest::fromJson(v.toObject()));
        p.rankThresholds = RankThresholds::fromJson(o["rank_thresholds"].toObject());
        return p;
    }
};

struct Progress {
    QSet<int>     completedDays;
    QSet<int>     stalledDays;        // days deliberately stalled (bridge streak gaps)
    QSet<QString> studyDates;         // ISO calendar dates on which ANY day was completed
    int           totalXp        = 0;
    int           streakCurrent  = 0;
    int           streakBest     = 0;
    QString       lastCompletedDate;

    QJsonObject toJson() const {
        QJsonObject o;
        QJsonArray arr;
        for (int d : completedDays) arr.append(d);
        o["completed_days"]      = arr;
        QJsonArray sarr;
        for (int d : stalledDays) sarr.append(d);
        o["stalled_days"]        = sarr;
        QJsonArray darr;
        for (const auto& s : studyDates) darr.append(s);
        o["study_dates"]         = darr;
        o["total_xp"]            = totalXp;
        o["streak_current"]      = streakCurrent;
        o["streak_best"]         = streakBest;
        o["last_completed_date"] = lastCompletedDate;
        return o;
    }
    static Progress fromJson(const QJsonObject& o) {
        Progress p;
        for (const auto& v : o["completed_days"].toArray())
            p.completedDays.insert(v.toInt());
        for (const auto& v : o["stalled_days"].toArray())
            p.stalledDays.insert(v.toInt());
        for (const auto& v : o["study_dates"].toArray())
            p.studyDates.insert(v.toString());
        p.totalXp            = o["total_xp"].toInt(0);
        p.streakCurrent      = o["streak_current"].toInt(0);
        p.streakBest         = o["streak_best"].toInt(0);
        p.lastCompletedDate  = o["last_completed_date"].toString();
        return p;
    }
};

// ============================================================
//  DATA MANAGER
// ============================================================
class DataManager {
public:
    static DataManager& instance() {
        static DataManager dm;
        return dm;
    }

    QVector<Plan>          plans;
    QMap<QString,Progress> progress;
    QSet<QString>          activePlanIds;   // multiple plans can be active simultaneously

    // Convenience: returns true if given plan is marked active
    bool isPlanActive(const QString& id) const { return activePlanIds.contains(id); }

    // Legacy single-active helper — returns first active or first plan
    QString primaryActivePlanId() const {
        if (!activePlanIds.isEmpty()) return *activePlanIds.begin();
        return plans.isEmpty() ? "" : plans[0].id;
    }

    void setActive(const QString& id, bool active) {
        if (active) activePlanIds.insert(id);
        else        activePlanIds.remove(id);
        save();
    }

    void load() {
        QDir().mkpath(dataDir());
        QFile f(dataPath());
        if (!f.open(QIODevice::ReadOnly)) return;
        auto doc = QJsonDocument::fromJson(f.readAll());
        f.close();
        if (doc.isNull()) return;
        auto root = doc.object();
        plans.clear();
        for (const auto& v : root["plans"].toArray())
            plans.append(Plan::fromJson(v.toObject()));
        progress.clear();
        auto prog = root["progress"].toObject();
        for (auto it = prog.begin(); it != prog.end(); ++it)
            progress[it.key()] = Progress::fromJson(it.value().toObject());

        // Support both old single-ID and new multi-ID formats
        activePlanIds.clear();
        if (root.contains("active_plan_ids")) {
            for (const auto& v : root["active_plan_ids"].toArray())
                activePlanIds.insert(v.toString());
        } else if (root.contains("active_plan_id")) {
            // Migrate from old single-active format
            QString id = root["active_plan_id"].toString();
            if (!id.isEmpty()) activePlanIds.insert(id);
        }
        // Default: if nothing stored, mark first non-archived plan as active
        if (activePlanIds.isEmpty() && !plans.isEmpty()) {
            for (const auto& p : plans)
                if (!p.archived) { activePlanIds.insert(p.id); break; }
        }
    }

    void save() const {
        QDir().mkpath(dataDir());
        QJsonObject root;
        QJsonArray pa;
        for (const auto& p : plans) pa.append(p.toJson());
        root["plans"] = pa;
        QJsonObject prog;
        for (auto it = progress.begin(); it != progress.end(); ++it)
            prog[it.key()] = it.value().toJson();
        root["progress"] = prog;
        // Save as array (new format) — old single-ID key kept for backward compat
        QJsonArray activeArr;
        for (const auto& id : activePlanIds) activeArr.append(id);
        root["active_plan_ids"] = activeArr;
        root["active_plan_id"]  = primaryActivePlanId(); // legacy compat
        QFile f(dataPath());
        if (!f.open(QIODevice::WriteOnly)) return;
        f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    }

    // Reverse a day's completion — subtracts XP, removes from set, recalculates streak
    void unmarkDay(const QString& planId, int day) {
        auto* plan = planById(planId);
        if (!plan) return;
        auto& prog = progressFor(planId);
        if (!prog.completedDays.contains(day)) return;

        const Quest* q = plan->questForDay(day);
        int xp = q ? q->xp : 100;

        prog.completedDays.remove(day);
        prog.totalXp = std::max(0, prog.totalXp - xp);

        // Rebuild studyDates from remaining completedDays.
        // We cannot keep studyDates as-is: if the user undoes all completions
        // on a given calendar date, that date must leave studyDates so the
        // streak correctly resets to 0. Rebuilding is the only safe approach.
        rebuildStudyDates(*plan, prog);

        recalcStreak(prog);
        save();
    }

    // Rebuild studyDates by mapping each remaining completedDay to its
    // calendar date via the plan's startDate. Called after unmarkDay so
    // studyDates always exactly reflects current completedDays.
    static void rebuildStudyDates(const Plan& plan, Progress& prog) {
        QDate start = QDate::fromString(plan.startDate, "yyyy-MM-dd");
        if (!start.isValid()) { prog.studyDates.clear(); return; }
        QSet<QString> rebuilt;
        for (int d : prog.completedDays) {
            QDate date = start.addDays(d - 1);
            rebuilt.insert(date.toString("yyyy-MM-dd"));
        }
        prog.studyDates = rebuilt;
    }

    // Mark any arbitrary day complete. Records the current calendar date as a study date.
    // Returns XP awarded (0 if day was already done).
    // countStreak: if false, today is NOT added to studyDates.
    // Use countStreak=false when the user explicitly opts out of
    // crediting today's streak for a backdated or pre-logged day.
    int markDay(const QString& planId, int day, bool countStreak = true) {
        auto* plan = planById(planId);
        if (!plan) return 0;
        auto& prog = progressFor(planId);
        if (prog.completedDays.contains(day)) return 0;

        // If it was stalled, un-stall it first (completing beats stalling)
        prog.stalledDays.remove(day);

        const Quest* q = plan->questForDay(day);
        int xp = q ? q->xp : 100;
        prog.completedDays.insert(day);
        prog.totalXp += xp;

        if (countStreak) {
            QString todayStr = QDate::currentDate().toString("yyyy-MM-dd");
            prog.studyDates.insert(todayStr);
            prog.lastCompletedDate = todayStr;
        }

        recalcStreak(prog);
        save();
        return xp;
    }

    // Stall a past missed day — bridges the streak gap without awarding XP
    bool stallDay(const QString& planId, int day) {
        auto& prog = progressFor(planId);
        if (prog.completedDays.contains(day)) return false; // can't stall a completed day
        if (prog.stalledDays.contains(day))   return false; // already stalled
        prog.stalledDays.insert(day);
        recalcStreak(prog);
        save();
        return true;
    }

    // Remove a stall from a day
    bool unstallDay(const QString& planId, int day) {
        auto& prog = progressFor(planId);
        if (!prog.stalledDays.contains(day)) return false;
        prog.stalledDays.remove(day);
        recalcStreak(prog);
        save();
        return true;
    }

private:
    // Recalculate streak from studyDates (calendar-based).
    // One study session per real calendar day = 1 streak point.
    // Stalled plan-days do NOT contribute to calendar streak.
    static void recalcStreak(Progress& prog) {
        if (prog.studyDates.isEmpty()) {
            prog.streakCurrent = 0;
            // streakBest is never reduced — it records the historical high
            return;
        }

        // Sort calendar dates
        QList<QDate> dates;
        for (const auto& s : prog.studyDates) {
            QDate d = QDate::fromString(s, "yyyy-MM-dd");
            if (d.isValid()) dates.append(d);
        }
        std::sort(dates.begin(), dates.end());

        // Best streak over all consecutive calendar-date runs
        int best = 1, cur = 1;
        for (int i = 1; i < dates.size(); i++) {
            if (dates[i] == dates[i-1].addDays(1)) {
                cur++;
                best = std::max(best, cur);
            } else {
                cur = 1;
            }
        }
        best = std::max(best, cur); // handle single-element
        // Exact recalc — streakBest reflects current data, not historical max.
        // This means undoing all days correctly resets best streak to 0.
        prog.streakBest = best;

        // Current streak = consecutive run ending at the most recent study date
        cur = 1;
        for (int i = dates.size() - 1; i > 0; i--) {
            if (dates[i] == dates[i-1].addDays(1)) cur++;
            else break;
        }

        // If the last study date was not today or yesterday, streak resets to 0
        QDate today = QDate::currentDate();
        QDate lastStudy = dates.last();
        if (lastStudy < today.addDays(-1)) {
            prog.streakCurrent = 0;
        } else {
            prog.streakCurrent = cur;
        }
    }

public:
    // Public so SetupTab can call after manual progress edits
    static void recalcStreakPublic(Progress& prog) { recalcStreak(prog); }

    Plan* planById(const QString& id) {
        for (auto& p : plans) if (p.id == id) return &p;
        return nullptr;
    }
    Plan* activePlan() { return planById(primaryActivePlanId()); }

    Progress& progressFor(const QString& planId) {
        if (!progress.contains(planId)) progress[planId] = Progress{};
        return progress[planId];
    }
    // Const version — returns default progress if plan has none yet
    const Progress& progressFor(const QString& planId) const {
        static const Progress empty{};
        auto it = progress.find(planId);
        return it != progress.end() ? it.value() : empty;
    }

    static QString dataDir() {
        return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }
    static QString dataPath() { return dataDir() + "/data.json"; }

private:
    DataManager() = default;
};

// ============================================================
//  UTILITY / RANK HELPERS
// ============================================================
namespace Util {

inline int todayDayNum(const Plan& plan) {
    QDate start = QDate::fromString(plan.startDate, "yyyy-MM-dd");
    if (!start.isValid()) return 1;
    return start.daysTo(QDate::currentDate()) + 1;
}

struct RankInfo {
    QString rank, label, color;
    int xp;
};

static const QVector<RankInfo> RANK_META = {
    {"E","Initiate",    Cl::ink4,   0},        // muted — just starting
    {"D","Novice",      Cl::teal,   1000},      // KA teal — first growth
    {"C","Apprentice",  Cl::blue,   3000},      // KA blue — building skill
    {"B","Practitioner",Cl::purple, 7000},      // KA purple — mastery building
    {"A","Expert",      Cl::gold,   15000},     // gold — near top
    {"S","Master",      Cl::navy,   30000},     // KA navy — highest prestige
};

static const QMap<QString,QString> RANK_MSG = {
    {"E","Your journey begins. Every expert was once a beginner."},
    {"D","You have taken your first real steps. Growth is happening."},
    {"C","The foundation is solid. You are someone who follows through."},
    {"B","Few reach this level. Consistency has become your identity."},
    {"A","You stand among those who finish what they start. Rare."},
    {"S","You have mastered reSTEM itself. This is dedication."},
};

inline QVector<RankInfo> effectiveRanks(const Plan& plan) {
    QVector<RankInfo> out = RANK_META;
    for (auto& r : out) r.xp = plan.rankThresholds.forRank(r.rank);
    return out;
}

inline RankInfo currentRank(int xp, const Plan& plan) {
    auto ranks = effectiveRanks(plan);
    RankInfo cur = ranks[0];
    for (const auto& r : ranks) {
        if (xp >= r.xp) cur = r;
        else break;
    }
    return cur;
}

inline std::optional<RankInfo> nextRank(int xp, const Plan& plan) {
    for (const auto& r : effectiveRanks(plan))
        if (xp < r.xp) return r;
    return std::nullopt;
}

// Convert plan day number to a short calendar date string, e.g. "Jul 14"
// Returns empty string if startDate is invalid.
inline QString planDateFor(const Plan& plan, int day) {
    QDate start = QDate::fromString(plan.startDate, "yyyy-MM-dd");
    if (!start.isValid() || day < 1) return "";
    return start.addDays(day - 1).toString("MMM d");
}

// Full ISO date for a plan day
inline QDate planQDateFor(const Plan& plan, int day) {
    QDate start = QDate::fromString(plan.startDate, "yyyy-MM-dd");
    if (!start.isValid() || day < 1) return QDate();
    return start.addDays(day - 1);
}
// If all days up to today are done, returns the last available day (all-caught-up state).
// If plan hasn't started yet, returns -1.
inline int nextPendingDay(const Plan& plan, const Progress& prog) {
    int todayNum = todayDayNum(plan);
    if (todayNum < 1) return -1;
    int cap = std::min(todayNum, plan.duration);
    for (int d = 1; d <= cap; d++) {
        if (!prog.completedDays.contains(d) && !prog.stalledDays.contains(d))
            return d;
    }
    // All available days done — return cap so caller can detect "caught up" state
    return cap;
}

struct XpProgInfo { int pct; QString label; };
inline XpProgInfo xpProgress(int xp, const Plan& plan) {
    auto cur = currentRank(xp, plan);
    auto nxt = nextRank(xp, plan);
    if (!nxt) return {100, "Max rank achieved"};
    int inLevel   = xp - cur.xp;
    int levelSize = nxt->xp - cur.xp;
    int pct = levelSize > 0 ? int(double(inLevel) / levelSize * 100) : 100;
    int toNext = nxt->xp - xp;
    return {pct, QString("%1 XP to %2 · %3")
            .arg(toNext).arg(nxt->rank).arg(nxt->label)};
}

inline int completionPct(const Plan& plan, const Progress& prog) {
    if (plan.duration <= 0) return 0;
    int done = int(prog.completedDays.size());
    // Only count days within current duration
    int validDone = 0;
    for (int d : prog.completedDays)
        if (d <= plan.duration) validDone++;
    return std::min(100, int(double(validDone) / plan.duration * 100));
}

// Power curve: rank thresholds scaled exponentially
inline RankThresholds powerCurve(int sXp, double exponent) {
    QStringList ranks = {"D","C","B","A","S"};
    RankThresholds t;
    t.E = 0;
    int prev = 0;
    int n = ranks.size();
    for (int i = 0; i < n; i++) {
        double ratio = double(i + 1) / n;
        int raw = int(sXp * std::pow(ratio, exponent));
        int val = std::max(raw, prev + 100);
        t.setRank(ranks[i], val);
        prev = val;
    }
    return t;
}

// Scale rank thresholds when total XP opportunity changes.
//
// Strategy:
//   1. Compute each rank's fraction of old S-rank threshold (its relative position)
//   2. Set new S = min(proportionally scaled S, totalXpNew) — NEVER exceed what's earnable
//   3. Derive D/C/B/A using the same fractions of new S
//
// This guarantees:
//   - Completing 100% of a plan can always reach S-rank (if you earn all XP)
//   - Relative difficulty curve (easy→hard) is preserved
//   - Strict ordering D < C < B < A < S is maintained
inline RankThresholds scaledThresholds(const RankThresholds& old,
                                        int totalXpOld, int totalXpNew) {
    if (totalXpOld <= 0 || totalXpNew <= 0) return old;

    // Step 1: proportional scaling ratio
    double ratio = double(totalXpNew) / totalXpOld;

    // Step 2: proposed S = proportionally scaled, then capped at totalXpNew
    int proposedS = int(old.S * ratio);
    int newS      = std::min(proposedS, totalXpNew);
    newS          = std::max(newS, 1); // safety: never 0

    // Step 3: derive other ranks as the same fraction of new S as they were of old S
    // fraction[r] = old.r / old.S  →  new.r = fraction[r] * newS
    QStringList ranks = {"D","C","B","A","S"};
    RankThresholds t;
    t.E = 0;
    int prev = 0;
    for (const auto& r : ranks) {
        int newVal;
        if (r == "S") {
            newVal = newS;
        } else if (old.S > 0) {
            // preserve relative position as fraction of S
            double frac = double(old.forRank(r)) / old.S;
            newVal = int(frac * newS);
        } else {
            newVal = int(old.forRank(r) * ratio);
        }
        newVal = std::max(newVal, prev + 50); // strict ordering, min 50 XP gap
        newVal = std::min(newVal, newS);      // never exceed S
        t.setRank(r, newVal);
        prev = newVal;
    }
    return t;
}

// Total XP available across all days in a plan (default 100 per day without explicit quest)
inline int totalPlanXP(const Plan& plan) {
    int total = 0;
    for (int d = 1; d <= plan.duration; d++) {
        const Quest* q = plan.questForDay(d);
        total += q ? q->xp : 100;
    }
    return total;
}

} // namespace Util

// ============================================================
//  CUSTOM WIDGET — XP Bar
// ============================================================
class XpBarWidget : public QWidget {
    Q_OBJECT
public:
    explicit XpBarWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setFixedHeight(7);
        setAttribute(Qt::WA_TranslucentBackground);
    }

    void setPct(int p) { m_pct = std::clamp(p, 0, 100); update(); }
    void setColor(const QString& c, const QString& c2 = "") {
        m_color  = c;
        m_color2 = c2.isEmpty() ? c : c2;
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        int w = width(), h = height(), r = h / 2;

        // Track — use a medium grey so the unfilled portion is clearly visible
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(Cl::border2));
        p.drawRoundedRect(0, 0, w, h, r, r);

        // Fill
        if (m_pct > 0) {
            int fw = int(w * m_pct / 100.0);
            QLinearGradient grad(0, 0, fw, 0);
            grad.setColorAt(0, QColor(m_color));
            grad.setColorAt(1, QColor(m_color2));
            p.setBrush(grad);
            p.drawRoundedRect(0, 0, fw, h, r, r);
        }
    }

private:
    int     m_pct    = 0;
    QString m_color  = Cl::blue;
    QString m_color2 = Cl::teal;
};

// ============================================================
//  CUSTOM WIDGET — Heatmap
// ============================================================
class HeatmapWidget : public QWidget {
    Q_OBJECT

    static constexpr int CELL = 15;
    static constexpr int GAP  = 3;
    static constexpr int COLS = 15;

public:
    explicit HeatmapWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setMouseTracking(true);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    }

    void setData(const Plan& plan, const Progress& prog) {
        m_plan = plan;
        m_prog = prog;
        int rows = (plan.duration + COLS - 1) / COLS;
        int w    = COLS * (CELL + GAP) + GAP;
        int h    = rows * (CELL + GAP) + GAP;
        setFixedSize(w, h);
        update();
    }

signals:
    void cellClicked(int day);

protected:
    void mouseMoveEvent(QMouseEvent* e) override {
        int d = dayAt(e->pos());
        if (d != m_hovered) { m_hovered = d; update(); }
        if (d > 0) {
            const Quest* q = m_plan.questForDay(d);
            QString dateStr = Util::planDateFor(m_plan, d);
            QString tip = dateStr.isEmpty()
                ? QString("Day %1").arg(d)
                : QString("Day %1  ·  %2").arg(d).arg(dateStr);
            if (q && !q->title.isEmpty()) tip += "\n" + q->title;
            if (q) tip += QString("  (+%1 XP)").arg(q->xp);
            if (m_prog.completedDays.contains(d))    tip += "  ✓ Done";
            else if (m_prog.stalledDays.contains(d)) tip += "  ⏸ Stalled";
            QToolTip::showText(e->globalPosition().toPoint(), tip, this);
        }
    }
    void leaveEvent(QEvent*) override { m_hovered = -1; update(); }
    void mousePressEvent(QMouseEvent* e) override {
        if (e->button() == Qt::LeftButton) {
            int d = dayAt(e->pos());
            if (d > 0) emit cellClicked(d);
        }
    }

    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        int todayNum = Util::todayDayNum(m_plan);

        for (int d = 1; d <= m_plan.duration; d++) {
            int col = (d - 1) % COLS;
            int row = (d - 1) / COLS;
            int x   = GAP + col * (CELL + GAP);
            int y   = GAP + row * (CELL + GAP);

            const Quest* q   = m_plan.questForDay(d);
            bool done        = m_prog.completedDays.contains(d);
            bool stalled     = m_prog.stalledDays.contains(d);
            bool isMs        = q && q->isMilestone;
            bool isToday     = d == todayNum;
            bool isFuture    = d > todayNum;

            QColor fill;
            if      (done && isMs)  fill = QColor(Cl::gold2);
            else if (done)          fill = QColor(Cl::green);
            else if (stalled)       fill = QColor(Cl::gold2);  // amber — stalled
            else if (isMs && !isFuture) fill = QColor(Cl::goldBg);
            else if (isFuture)      fill = QColor(Cl::cardBg2);   // future — KA light grey
            else                    fill = QColor(Cl::cardBg3);   // pending (past, not done)

            if (d == m_hovered) fill = fill.lighter(110);

            QColor borderClr = (isMs && !done) ? QColor(Cl::gold2) : QColor(Cl::border);
            double borderW   = (isMs && !done) ? 1.5 : 0.5;

            p.setPen(QPen(borderClr, borderW));
            p.setBrush(fill);
            p.drawRoundedRect(x, y, CELL, CELL, 3, 3);

            // Today ring — KA blue
            if (isToday) {
                p.setPen(QPen(QColor(Cl::blue), 2.0));
                p.setBrush(Qt::NoBrush);
                p.drawRoundedRect(x-2, y-2, CELL+4, CELL+4, 4, 4);
            }
        }
    }

private:
    Plan     m_plan;
    Progress m_prog;
    int      m_hovered = -1;

    int dayAt(const QPoint& pos) const {
        int c = pos.x() / (CELL + GAP);
        int r = pos.y() / (CELL + GAP);
        int d = r * COLS + c + 1;
        return (d >= 1 && d <= m_plan.duration) ? d : -1;
    }
};

// ============================================================
//  CUSTOM WIDGET — Stat Card
// ============================================================
class StatCard : public QFrame {
public:
    StatCard(const QString& value, const QString& label, const QString& color,
             QWidget* parent = nullptr)
        : QFrame(parent)
    {
        setStyleSheet(QString("QFrame { background: %1; border: 1px solid %2; border-radius: 12px; }")
                      .arg(Cl::white_).arg(Cl::border));
        setMinimumWidth(140);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        auto* vl = new QVBoxLayout(this);
        vl->setContentsMargins(14, 12, 14, 12);
        vl->setSpacing(3);
        vl->setAlignment(Qt::AlignCenter);

        auto* val = new QLabel(value);
        val->setStyleSheet(QString("color: %1; font-size: 24px; font-weight: 700; border: none; background: transparent;").arg(color));
        val->setAlignment(Qt::AlignCenter);

        auto* lbl = new QLabel(label.toUpper());
        lbl->setStyleSheet(QString("color: %1; font-size: 9px; letter-spacing: 3px; border: none; background: transparent;").arg(Cl::ink4));
        lbl->setAlignment(Qt::AlignCenter);

        vl->addWidget(val);
        vl->addWidget(lbl);
    }
};

// ============================================================
//  DETAILS DIALOG
// ============================================================
class DetailsDialog : public QDialog {
    Q_OBJECT
public:
    explicit DetailsDialog(const QString& planId, QWidget* parent = nullptr)
        : QDialog(parent), m_planId(planId)
    {
        auto* plan = DataManager::instance().planById(planId);
        setWindowTitle(plan ? "Details — " + plan->name : "Details");
        setMinimumSize(640, 480);
        // Size to 70% of screen, max 920×720
        QScreen* scr = QApplication::primaryScreen();
        QSize avail = scr ? scr->availableSize() : QSize(1280, 800);
        resize(std::min(int(avail.width() * 0.70), 920),
               std::min(int(avail.height() * 0.75), 720));

        auto* root = new QVBoxLayout(this);
        root->setContentsMargins(0, 0, 0, 0);

        auto* tabs = new QTabWidget;
        tabs->addTab(buildOverviewTab(), "  Overview  ");
        tabs->addTab(buildMapTab(),      "  Map  ");
        tabs->addTab(buildMsTab(),       "  Milestones  ");
        tabs->addTab(buildAchTab(),      "  Achievements  ");
        root->addWidget(tabs);

        auto* close = new QPushButton("Close");
        close->setProperty("role", "primary");
        close->setFixedWidth(100);
        connect(close, &QPushButton::clicked, this, &QDialog::accept);
        auto* br = new QHBoxLayout;
        br->setContentsMargins(20, 10, 20, 14);
        br->addStretch(); br->addWidget(close);
        root->addLayout(br);
    }

signals:
    void dataChanged();

private:
    QString m_planId;

    std::pair<Plan*, Progress*> pp() {
        auto& dm = DataManager::instance();
        return {dm.planById(m_planId), &dm.progressFor(m_planId)};
    }

    // ── Overview ──────────────────────────────────
    QWidget* buildOverviewTab() {
        auto* w = new QWidget;
        auto* scroll = new QScrollArea;
        scroll->setWidgetResizable(true); scroll->setWidget(w);
        auto* vl = new QVBoxLayout(w);
        vl->setContentsMargins(24, 20, 24, 20); vl->setSpacing(14);

        auto [plan, prog] = pp();
        if (!plan) return scroll;

        int xp   = prog->totalXp;
        auto rank = Util::currentRank(xp, *plan);
        auto xi   = Util::xpProgress(xp, *plan);
        int pct   = Util::completionPct(*plan, *prog);

        // Rank badge
        auto* rankLbl = new QLabel(QString("  %1-Rank · %2  ").arg(rank.rank).arg(rank.label));
        rankLbl->setStyleSheet(QString("color: %1; border: 2px solid %1; border-radius: 6px;"
            " font-size: 12px; font-weight: 700; letter-spacing: 3px; padding: 5px 12px;"
            " background: %2;").arg(rank.color).arg(Cl::white_));
        auto* topRow = new QHBoxLayout;
        topRow->addWidget(rankLbl); topRow->addStretch();
        vl->addLayout(topRow);

        // XP group
        auto* xpGrp = new QGroupBox("XP PROGRESSION");
        auto* xpVl  = new QVBoxLayout(xpGrp);
        auto* xpRow = new QHBoxLayout;
        auto* xpL   = new QLabel(QString("%1 XP total").arg(xp));
        xpL->setStyleSheet("border: none; background: transparent;");
        auto* xpR   = new QLabel(xi.label);
        xpR->setStyleSheet(QString("color: %1; border: none; background: transparent;").arg(Cl::ink4));
        xpRow->addWidget(xpL); xpRow->addStretch(); xpRow->addWidget(xpR);
        xpVl->addLayout(xpRow);
        auto* bar = new XpBarWidget;
        bar->setFixedHeight(8);
        bar->setPct(xi.pct);
        bar->setColor(Cl::blue, Cl::teal);
        xpVl->addWidget(bar);
        vl->addWidget(xpGrp);

        // Stats grid
        auto* sGrp  = new QGroupBox("STATS");
        auto* sGrid = new QGridLayout(sGrp);
        sGrid->setSpacing(10);
        struct SC { QString v, l, c; };
        int validDone = Plan::validCompletedCount(prog->completedDays, plan->duration);
        QVector<SC> stats = {
            {QString("%1%").arg(std::min(100, pct)), "Completion",  Cl::blue},
            {QString::number(validDone),              "Days Done",   Cl::green},
            {QString::number(prog->streakCurrent),    "Streak",      Cl::green2},
            {QString::number(prog->streakBest),       "Best Streak", Cl::green},
            {QString("%1").arg(xp),                   "Total XP",    Cl::purple2},
            {QString::number(std::max(0, plan->duration - validDone)), "Remaining", Cl::ink3},
        };
        for (int i = 0; i < stats.size(); i++)
            sGrid->addWidget(new StatCard(stats[i].v, stats[i].l, stats[i].c), i/3, i%3);
        vl->addWidget(sGrp);

        // Rank progression
        auto* rGrp = new QGroupBox("RANK PROGRESSION");
        auto* rVl  = new QVBoxLayout(rGrp);
        auto ranks = Util::effectiveRanks(*plan);
        for (const auto& r : ranks) {
            bool unlocked = xp >= r.xp;
            bool isCur    = rank.rank == r.rank;
            auto* row = new QWidget;
            row->setStyleSheet("background: transparent; border: none;");
            auto* rl = new QHBoxLayout(row);
            rl->setContentsMargins(0, 3, 0, 3);

            auto* badge = new QLabel(QString("  %1  ").arg(r.rank));
            badge->setStyleSheet(QString("color: %1; border: 2px solid %1; border-radius: 5px;"
                " font-size: 11px; font-weight: 700; padding: 2px 8px;"
                " background: %2;").arg(unlocked ? r.color : Cl::ink4).arg(Cl::white_));
            auto* name = new QLabel(r.label);
            name->setStyleSheet(QString("color: %1; font-weight: 600; border: none; background: transparent;")
                                .arg(unlocked ? r.color : Cl::ink4));
            auto* xpL2 = new QLabel(QString("%1 XP").arg(r.xp));
            xpL2->setStyleSheet(QString("color: %1; font-size: 11px; border: none; background: transparent;").arg(Cl::ink4));
            auto* curL = new QLabel(isCur ? "← current" : (unlocked ? "✓" : ""));
            curL->setStyleSheet(QString("color: %1; font-size: 10px; border: none; background: transparent;").arg(Cl::ink4));

            rl->addWidget(badge); rl->addWidget(name); rl->addStretch();
            rl->addWidget(xpL2); rl->addSpacing(12); rl->addWidget(curL);

            auto* sep = new QFrame;
            sep->setFrameShape(QFrame::HLine);
            sep->setStyleSheet(QString("background: %1; max-height: 1px; border: none;").arg(Cl::border));
            rVl->addWidget(row); rVl->addWidget(sep);
        }
        vl->addWidget(rGrp);

        // ── Topics Cleared (strikethrough list) ──────────────────────
        // Count valid completed days (within duration)
        QVector<QPair<int,QString>> cleared; // (day, title)
        for (int d : prog->completedDays) {
            if (d > plan->duration) continue; // skip orphans
            const Quest* q = plan->questForDay(d);
            QString title = (q && !q->title.isEmpty()) ? q->title
                          : QString("Day %1").arg(d);
            cleared.append({d, title});
        }
        std::sort(cleared.begin(), cleared.end(),
            [](const auto& a, const auto& b){ return a.first < b.first; });

        if (!cleared.isEmpty()) {
            auto* tGrp = new QGroupBox(
                QString("TOPICS CLEARED  (%1 / %2)")
                    .arg(cleared.size()).arg(plan->duration));
            auto* tVl  = new QVBoxLayout(tGrp);
            tVl->setSpacing(4);

            for (const auto& [day, title] : cleared) {
                const Quest* q = plan->questForDay(day);
                bool isMs = q && q->isMilestone;

                auto* row = new QWidget;
                row->setStyleSheet("background: transparent; border: none;");
                auto* rl = new QHBoxLayout(row);
                rl->setContentsMargins(4, 2, 4, 2); rl->setSpacing(10);

                // Checkbox-style tick icon
                auto* tick = new QLabel("✓");
                tick->setStyleSheet(QString(
                    "color: %1; font-size: 12px; font-weight: 700;"
                    " border: none; background: transparent; min-width: 16px;")
                    .arg(isMs ? Cl::gold2 : Cl::teal));
                tick->setAlignment(Qt::AlignCenter);

                // Day number
                auto* dayLbl = new QLabel(QString("Day %1").arg(day));
                dayLbl->setStyleSheet(QString(
                    "color: %1; font-size: 10px; min-width: 44px;"
                    " border: none; background: transparent;").arg(Cl::ink4));

                // Strikethrough title — use HTML rich text
                QString msTag = isMs ? " 🏆" : "";
                auto* titleLbl = new QLabel(
                    QString("<s>%1%2</s>").arg(title.toHtmlEscaped()).arg(msTag));
                titleLbl->setTextFormat(Qt::RichText);
                titleLbl->setStyleSheet(QString(
                    "color: %1; font-size: 12px; border: none;"
                    " background: transparent;").arg(Cl::ink3));
                titleLbl->setWordWrap(true);
                titleLbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

                // XP badge
                auto* xpLbl = new QLabel(
                    q ? QString("+%1 xp").arg(q->xp) : "");
                xpLbl->setStyleSheet(QString(
                    "color: %1; font-size: 10px; border: none;"
                    " background: transparent;").arg(Cl::ink4));

                rl->addWidget(tick);
                rl->addWidget(dayLbl);
                rl->addWidget(titleLbl, 1);
                rl->addWidget(xpLbl);

                // Light separator
                auto* sep = new QFrame;
                sep->setFrameShape(QFrame::HLine);
                sep->setStyleSheet(QString(
                    "background: %1; max-height: 1px; border: none;").arg(Cl::border));

                tVl->addWidget(row);
                if (&cleared.back() != &cleared[cleared.indexOf({day, title})])
                    tVl->addWidget(sep);
            }
            vl->addWidget(tGrp);
        }

        vl->addStretch();

        auto* outer = new QWidget;
        auto* ol = new QVBoxLayout(outer);
        ol->setContentsMargins(0,0,0,0);
        ol->addWidget(scroll);
        return outer;
    }

    // ── Map ─────────────────────────────────────
    QWidget* buildMapTab() {
        auto [plan, prog] = pp();
        auto* w  = new QWidget;
        auto* vl = new QVBoxLayout(w);
        vl->setContentsMargins(24, 18, 24, 18); vl->setSpacing(12);

        if (!plan) return w;

        auto* hmap = new HeatmapWidget;
        hmap->setData(*plan, *prog);
        connect(hmap, &HeatmapWidget::cellClicked, this, [this, hmap](int day) {
            auto& dm   = DataManager::instance();
            auto* plan = dm.planById(m_planId);
            if (!plan) return;
            // Silently ignore clicks beyond plan duration — those cells are decorative
            if (day < 1 || day > plan->duration) return;
            auto& prog    = dm.progressFor(m_planId);
            int todayNum  = Util::todayDayNum(*plan);
            const Quest* q = plan->questForDay(day);
            QString topicLine = (q && !q->title.isEmpty())
                ? QString("\n\"%1\"").arg(q->title) : "";
            bool done    = prog.completedDays.contains(day);
            bool stalled = prog.stalledDays.contains(day);

            if (done) {
                // ── Completed → Undo ────────────────────────────────
                int xp = q ? q->xp : 100;
                auto* dlg = new QMessageBox(this);
                dlg->setWindowTitle(QString("Day %1 — Completed").arg(day));
                dlg->setText(QString(
                    "Day %1%2\n\n"
                    "This day is marked complete (+%3 XP).\n"
                    "Do you want to undo this completion?")
                    .arg(day).arg(topicLine).arg(xp));
                dlg->setIcon(QMessageBox::Question);
                auto* undoBtn = dlg->addButton("↩  Undo Completion", QMessageBox::DestructiveRole);
                dlg->addButton("Keep It", QMessageBox::RejectRole);
                dlg->exec();
                if (dlg->clickedButton() == undoBtn) {
                    dm.unmarkDay(m_planId, day);
                    hmap->setData(*plan, prog);
                    emit dataChanged();
                }

            } else if (stalled) {
                // ── Stalled → Unstall or upgrade to Complete ────────
                auto* dlg = new QMessageBox(this);
                dlg->setWindowTitle(QString("Day %1 — Stalled").arg(day));
                dlg->setText(QString(
                    "Day %1%2\n\n"
                    "This day is stalled — it bridges your streak "
                    "without awarding XP.\n\n"
                    "What would you like to do?")
                    .arg(day).arg(topicLine));
                dlg->setIcon(QMessageBox::Question);
                auto* markBtn   = dlg->addButton("✓  Mark Complete Instead", QMessageBox::AcceptRole);
                auto* removeBtn = dlg->addButton("↩  Remove Stall",          QMessageBox::DestructiveRole);
                dlg->addButton("Keep Stall", QMessageBox::RejectRole);
                dlg->exec();
                if (dlg->clickedButton() == markBtn) {
                    // For stalled days, also ask about streak
                    bool todayStall = (day == todayNum);
                    bool countStreakStall = todayStall; // default: count if today
                    if (!todayStall) {
                        auto* sAsk = new QMessageBox(this);
                        sAsk->setWindowTitle("Count toward streak?");
                        sAsk->setText(QString(
                            "Day %1 is a past day.\n\n"
                            "Do you want today to count toward your streak?")
                            .arg(day));
                        auto* sYes = sAsk->addButton("Yes, Count It",  QMessageBox::AcceptRole);
                        sAsk->addButton("No, Just Log It", QMessageBox::RejectRole);
                        sAsk->exec();
                        countStreakStall = (sAsk->clickedButton() == sYes);
                    }
                    dm.unstallDay(m_planId, day);
                    int xp = dm.markDay(m_planId, day, countStreakStall);
                    hmap->setData(*plan, prog);
                    QMessageBox::information(this, "Marked Complete",
                        QString("Day %1 marked complete.\n+%2 XP awarded.%3")
                        .arg(day).arg(xp)
                        .arg(todayStall ? "" : (countStreakStall
                            ? "\nStreak counted." : "\nStreak unchanged.")));
                    emit dataChanged();
                } else if (dlg->clickedButton() == removeBtn) {
                    dm.unstallDay(m_planId, day);
                    hmap->setData(*plan, prog);
                    emit dataChanged();
                }

            } else if (day > todayNum) {
                // ── Future → Study-ahead with streak choice ──────────
                int daysAhead = day - todayNum;
                auto* dlg = new QMessageBox(this);
                dlg->setWindowTitle(QString("Day %1 — Study Ahead").arg(day));
                dlg->setText(QString(
                    "Day %1%2\n\n"
                    "This is %3 day%4 ahead of today.\n"
                    "Did you study ahead and complete this topic?")
                    .arg(day).arg(topicLine)
                    .arg(daysAhead).arg(daysAhead == 1 ? "" : "s"));
                dlg->setIcon(QMessageBox::Question);
                auto* markYesStreak = dlg->addButton("✓  Yes, Count Streak",    QMessageBox::AcceptRole);
                auto* markNoStreak  = dlg->addButton("✓  Yes, Don't Count Streak", QMessageBox::ActionRole);
                dlg->addButton("Cancel", QMessageBox::RejectRole);
                dlg->setDetailedText(
                    "Count Streak: today is recorded as a study day.\n"
                    "Don't Count Streak: day is marked complete (XP awarded)\n"
                    "  but your streak is not affected.");
                dlg->exec();
                bool doMark       = (dlg->clickedButton() == markYesStreak ||
                                     dlg->clickedButton() == markNoStreak);
                bool countStreak_ = (dlg->clickedButton() == markYesStreak);
                if (doMark) {
                    int xp = dm.markDay(m_planId, day, countStreak_);
                    hmap->setData(*plan, prog);
                    QMessageBox::information(this, "Marked Complete",
                        QString("Day %1 marked complete.\n+%2 XP awarded.\n%3")
                        .arg(day).arg(xp)
                        .arg(countStreak_ ? "Streak counted." : "Streak unchanged."));
                    emit dataChanged();
                }

            } else {
                // ── Past / today, pending → Mark Complete or Stall ──
                bool isToday_ = (day == todayNum);
                auto* dlg = new QMessageBox(this);
                dlg->setWindowTitle(QString("Day %1").arg(day));
                dlg->setText(QString("Day %1%2\n\nChoose an action:")
                    .arg(day).arg(topicLine));
                dlg->setIcon(QMessageBox::Question);

                QPushButton* markYesStreak = nullptr;
                QPushButton* markNoStreak  = nullptr;
                QPushButton* markBtn       = nullptr;

                if (isToday_) {
                    // Today — single "Mark Complete" button, always counts streak
                    markBtn = dlg->addButton("✓  Mark Complete", QMessageBox::AcceptRole);
                } else {
                    // Past day — offer streak choice
                    markYesStreak = dlg->addButton("✓  Mark Complete (Count Streak)",
                                                   QMessageBox::AcceptRole);
                    markNoStreak  = dlg->addButton("✓  Mark Complete (Keep Streak)",
                                                   QMessageBox::ActionRole);
                    dlg->setDetailedText(
                        "Count Streak: today is recorded as a study day.\n"
                        "Keep Streak: day is marked complete (XP awarded)\n"
                        "  but your streak is not affected.");
                }
                auto* stallBtn = dlg->addButton("  Stall This Day", QMessageBox::ActionRole);
                dlg->addButton("Cancel", QMessageBox::RejectRole);
                dlg->exec();

                bool clickedMark = (markBtn && dlg->clickedButton() == markBtn)
                                || (markYesStreak && dlg->clickedButton() == markYesStreak)
                                || (markNoStreak  && dlg->clickedButton() == markNoStreak);
                bool countStreak_ = isToday_
                                 || (markYesStreak && dlg->clickedButton() == markYesStreak);

                if (clickedMark) {
                    int xp = dm.markDay(m_planId, day, countStreak_);
                    hmap->setData(*plan, prog);
                    QMessageBox::information(this, "Marked Complete",
                        QString("Day %1 marked complete.\n+%2 XP awarded.%3")
                        .arg(day).arg(xp)
                        .arg(isToday_ ? "" : (countStreak_
                            ? "\nStreak counted." : "\nStreak unchanged.")));
                    emit dataChanged();
                } else if (dlg->clickedButton() == stallBtn) {
                    dm.stallDay(m_planId, day);
                    hmap->setData(*plan, prog);
                    QMessageBox::information(this, "Day Stalled",
                        QString("Day %1 stalled.\n\n"
                                "Your streak is preserved. No XP awarded.\n"
                                "Click the cell again to remove the stall or mark it complete.")
                        .arg(day));
                    emit dataChanged();
                }
            }
        });

        // Legend
        auto* legRow = new QHBoxLayout;
        struct LI { QString col, lbl; };
        QVector<LI> legend = {
            {Cl::green,  "Done"},
            {Cl::gold2,  "Milestone done"},
            {Cl::gold2,  "Stalled"},
            {Cl::cardBg3, "Pending"},
            {Cl::cardBg2, "Future"},
        };
        for (const auto& li : legend) {
            auto* dot = new QLabel("  ");
            dot->setFixedSize(13, 13);
            dot->setStyleSheet(QString("background: %1; border-radius: 3px; border: 1px solid %2;")
                               .arg(li.col).arg(Cl::border));
            auto* lbl = new QLabel(li.lbl);
            lbl->setStyleSheet(QString("color: %1; font-size: 11px; border: none;").arg(Cl::ink4));
            legRow->addWidget(dot); legRow->addWidget(lbl); legRow->addSpacing(10);
        }
        legRow->addStretch();

        vl->addLayout(legRow);

        // Put hmap in a scroll area that grows with it, left-aligned, no stretch gap
        auto* sc = new QScrollArea;
        sc->setWidgetResizable(true);
        sc->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        auto* container = new QWidget;
        container->setStyleSheet(QString("background: %1;").arg(Cl::cream));
        auto* cl = new QVBoxLayout(container);
        cl->setContentsMargins(0, 0, 0, 0);
        cl->setSpacing(0);
        cl->addWidget(hmap, 0, Qt::AlignTop | Qt::AlignLeft);
        cl->addStretch(1);
        sc->setWidget(container);
        vl->addWidget(sc, 1);
        return w;
    }

    // ── Milestones ──────────────────────────────
    QWidget* buildMsTab() {
        auto [plan, prog] = pp();
        auto* w     = new QWidget;
        auto* scroll = new QScrollArea;
        scroll->setWidgetResizable(true); scroll->setWidget(w);
        auto* vl = new QVBoxLayout(w);
        vl->setContentsMargins(24, 18, 24, 18); vl->setSpacing(10);

        if (!plan) { auto* o = new QWidget; auto* l = new QVBoxLayout(o); l->addWidget(scroll); return o; }

        QVector<const Quest*> milestones;
        for (const auto& q : plan->quests)
            if (q.isMilestone && q.day <= plan->duration) milestones.append(&q);
        std::sort(milestones.begin(), milestones.end(),
                  [](const Quest* a, const Quest* b){ return a->day < b->day; });

        if (milestones.isEmpty()) {
            auto* empty = new QLabel("No milestones set.\nGo to Setup tab and toggle a day as 🏆 checkpoint.");
            empty->setAlignment(Qt::AlignCenter);
            empty->setStyleSheet(QString("color: %1; font-size: 13px; border: none;").arg(Cl::ink4));
            vl->addStretch(); vl->addWidget(empty); vl->addStretch();
        } else {
            for (const auto* ms : milestones) {
                bool done = prog->completedDays.contains(ms->day);
                auto* card = new QFrame;
                card->setStyleSheet(QString("QFrame { background: %1; border: 1px solid %2; border-radius: 12px; }")
                    .arg(done ? Cl::greenBg : Cl::white_)
                    .arg(done ? Cl::greenBdr : Cl::border));
                auto* cl = new QHBoxLayout(card);
                cl->setContentsMargins(16, 12, 16, 12);

                auto* icon = makeIcon(done ? "rank" : "pause", 22,
                    QColor(done ? Cl::gold2 : Cl::ink4));
                icon->setStyleSheet("font-size: 20px; border: none; background: transparent;");

                auto* nameCol = new QVBoxLayout;
                auto* nameLbl = new QLabel(!ms->milestoneTitle.isEmpty() ? ms->milestoneTitle : ms->title);
                nameLbl->setStyleSheet(QString("color: %1; font-weight: 600; font-size: 13px; border: none; background: transparent;")
                    .arg(done ? Cl::green2 : Cl::ink3));
                auto* dayLbl = new QLabel(QString("Day %1  ·  +%2 XP").arg(ms->day).arg(ms->xp));
                dayLbl->setStyleSheet(QString("color: %1; font-size: 11px; border: none; background: transparent;").arg(Cl::ink4));
                nameCol->addWidget(nameLbl); nameCol->addWidget(dayLbl);

                auto* status = new QLabel(done ? "CLEARED ✓" : "LOCKED");
                status->setStyleSheet(QString("color: %1; font-size: 10px; font-weight: 700; letter-spacing: 2px; border: none; background: transparent;")
                    .arg(done ? Cl::green2 : Cl::ink4));

                cl->addWidget(icon); cl->addLayout(nameCol); cl->addStretch(); cl->addWidget(status);
                vl->addWidget(card);
            }
        }
        vl->addStretch();
        auto* outer = new QWidget; auto* ol = new QVBoxLayout(outer);
        ol->setContentsMargins(0,0,0,0); ol->addWidget(scroll);
        return outer;
    }

    // ── Achievements ────────────────────────────
    QWidget* buildAchTab() {
        auto [plan, prog] = pp();
        auto* w     = new QWidget;
        auto* scroll = new QScrollArea;
        scroll->setWidgetResizable(true); scroll->setWidget(w);
        auto* vl = new QVBoxLayout(w);
        vl->setContentsMargins(24, 18, 24, 18); vl->setSpacing(12);

        struct AchDef { QString id, icon, title, desc; };
        static const QVector<AchDef> defs = {
            {"first_step",  "🌱","First Step",       "Complete Day 1"},
            {"streak_3",    "🔥","On Fire",           "3-day streak"},
            {"streak_7",    "⚔️","Week Warrior",      "7-day streak"},
            {"streak_14",   "🗡️","Fortnight",         "14-day streak"},
            {"streak_30",   "👑","Month Master",      "30-day streak"},
            {"pct_25",      "📍","Quarter Way",       "25% complete"},
            {"pct_50",      "⚡","Halfway Hero",      "50% complete"},
            {"pct_75",      "🏹","Three Quarters",    "75% complete"},
            {"pct_100",     "🏆","Champion",          "100% complete"},
            {"rank_d",      "🟢","D-Rank",            "Reach D-Rank"},
            {"rank_c",      "🔵","C-Rank",            "Reach C-Rank"},
            {"rank_b",      "🟣","B-Rank",            "Reach B-Rank"},
            {"rank_a",      "🟠","A-Rank",            "Reach A-Rank"},
            {"rank_s",      "🔴","S-Rank",            "Reach S-Rank"},
            {"milestone_1", "🎯","Checkpoint",        "Clear 1st milestone"},
            {"milestone_5", "🗺️","Waypointer",       "Clear 5 milestones"},
        };

        if (!plan) { auto* o = new QWidget; auto* l = new QVBoxLayout(o); l->addWidget(scroll); return o; }

        int xp  = prog->totalXp;
        int pct = Util::completionPct(*plan, *prog);
        int msCleared = 0;
        for (const auto& q : plan->quests)
            if (q.isMilestone && prog->completedDays.contains(q.day)) msCleared++;
        auto thr = plan->rankThresholds;

        auto earned = [&](const QString& id) -> bool {
            if (id=="first_step")  return prog->completedDays.contains(1);
            if (id=="streak_3")    return prog->streakBest >= 3;
            if (id=="streak_7")    return prog->streakBest >= 7;
            if (id=="streak_14")   return prog->streakBest >= 14;
            if (id=="streak_30")   return prog->streakBest >= 30;
            if (id=="pct_25")      return pct >= 25;
            if (id=="pct_50")      return pct >= 50;
            if (id=="pct_75")      return pct >= 75;
            if (id=="pct_100")     return pct >= 100;
            if (id=="rank_d")      return xp >= thr.D;
            if (id=="rank_c")      return xp >= thr.C;
            if (id=="rank_b")      return xp >= thr.B;
            if (id=="rank_a")      return xp >= thr.A;
            if (id=="rank_s")      return xp >= thr.S;
            if (id=="milestone_1") return msCleared >= 1;
            if (id=="milestone_5") return msCleared >= 5;
            return false;
        };

        int earnedCount = 0;
        for (const auto& d : defs) if (earned(d.id)) earnedCount++;
        auto* sumLbl = new QLabel(QString("%1 / %2 achievements earned").arg(earnedCount).arg(defs.size()));
        sumLbl->setStyleSheet(QString("color: %1; font-size: 12px; border: none;").arg(Cl::ink3));
        vl->addWidget(sumLbl);

        auto* grid = new QGridLayout;
        grid->setSpacing(10);
        for (int i = 0; i < defs.size(); i++) {
            const auto& d = defs[i];
            bool e = earned(d.id);
            auto* card = new QFrame;
            card->setStyleSheet(QString("QFrame { background: %1; border: 1px solid %2; border-radius: 12px; }")
                .arg(e ? Cl::greenBg : Cl::cardBg2)
                .arg(e ? Cl::greenBdr : Cl::border));
            auto* cl = new QVBoxLayout(card);
            cl->setContentsMargins(12,12,12,12);
            cl->setAlignment(Qt::AlignCenter);

            // Unlocked: show emoji badge. Locked: show pause SVG (lock shape)
            // Use QLabel* throughout so setAlignment() is always available.
            QLabel* iconL = nullptr;
            if (e) {
                iconL = new QLabel(d.icon);
                iconL->setStyleSheet("font-size: 22px; border: none; background: transparent;");
            } else {
                iconL = makeIcon("pause", 22, QColor(Cl::ink4));
            }
            iconL->setStyleSheet("font-size: 26px; border: none; background: transparent;");
            iconL->setAlignment(Qt::AlignCenter);
            auto* titleL = new QLabel(d.title);
            titleL->setStyleSheet(QString("color: %1; font-size: 11px; font-weight: 700; border: none; background: transparent;")
                .arg(e ? Cl::green2 : Cl::ink4));
            titleL->setAlignment(Qt::AlignCenter);
            auto* descL = new QLabel(d.desc);
            descL->setStyleSheet(QString("color: %1; font-size: 10px; border: none; background: transparent;").arg(Cl::ink4));
            descL->setAlignment(Qt::AlignCenter);
            descL->setWordWrap(true);
            cl->addWidget(iconL); cl->addWidget(titleL); cl->addWidget(descL);
            cl->addStretch();
            card->setMinimumHeight(100);
            grid->addWidget(card, i/3, i%3);
        }
        vl->addLayout(grid);
        vl->addStretch();

        auto* outer = new QWidget; auto* ol = new QVBoxLayout(outer);
        ol->setContentsMargins(0,0,0,0); ol->addWidget(scroll);
        return outer;
    }
};

// ============================================================
//  QUEST CARD — collapsible card for dashboard
// ============================================================
class QuestCard : public QFrame {
    Q_OBJECT
public:
    explicit QuestCard(const QString& planId, QWidget* parent = nullptr)
        : QFrame(parent), m_planId(planId)
    {
        setStyleSheet(QString("QFrame { background: %1; border: 1px solid %2; border-radius: 14px; }")
                      .arg(Cl::white_).arg(Cl::border));

        m_root = new QVBoxLayout(this);
        m_root->setContentsMargins(0, 0, 0, 0);
        m_root->setSpacing(0);
        // Ensure child widgets are clipped to card's rounded boundary
        setAutoFillBackground(false);

        buildHeader();
        buildBody();
    }

    void refresh() {
        auto* plan = DataManager::instance().planById(m_planId);
        if (!plan) return;
        auto& prog = DataManager::instance().progressFor(m_planId);

        int xp       = prog.totalXp;
        auto rank    = Util::currentRank(xp, *plan);
        auto xi      = Util::xpProgress(xp, *plan);
        int pct      = Util::completionPct(*plan, prog);
        int streak   = prog.streakCurrent;
        int todayNum = Util::todayDayNum(*plan);

        // activeDay: the next quest the user should work on
        // = lowest uncompleted/unstalled day up to today
        int activeDay   = Util::nextPendingDay(*plan, prog);
        bool notStarted = todayNum < 1;
        bool finished   = todayNum > plan->duration;
        bool allDone    = !notStarted && !finished && activeDay == std::min(todayNum, plan->duration)
                          && prog.completedDays.contains(activeDay);
        bool activeDone = activeDay > 0 && prog.completedDays.contains(activeDay);
        const Quest* q  = (activeDay > 0) ? plan->questForDay(activeDay) : nullptr;
        bool isMs       = q && q->isMilestone;

        // ── Row 1: plan name + rank ────────────────────────────────
        m_titleLbl->setText(plan->name);
        m_rankLbl->setText(QString("%1-Rank · %2").arg(rank.rank).arg(rank.label));
        m_rankLbl->setStyleSheet(QString(
            "color: %1; font-size: 11px; font-weight: 700;"
            " letter-spacing: 2px; border: none; background: transparent;")
            .arg(rank.color));

        // ── Row 2: XP bar ─────────────────────────────────────────
        m_xpBar->setPct(xi.pct);
        m_xpBar->setColor(Cl::blue, Cl::teal);

        // ── Row 3: Active quest panel ─────────────────────────────
        if (notStarted) {
            m_questDayLbl->setText("NOT STARTED YET");
            m_questTitleHdr->setText(QString("Starts %1").arg(plan->startDate));
            m_todayPanel->setStyleSheet(QString(
                "QFrame { background: %1; border: 1px solid %2;"
                " border-left: 4px solid %3; border-radius: 10px; }")
                .arg(Cl::cardBg2).arg(Cl::border).arg(Cl::ink4));
            m_questDayLbl->setStyleSheet(QString(
                "color: %1; font-size: 10px; font-weight: 600;"
                " letter-spacing: 2px; border: none; background: transparent;").arg(Cl::ink4));
            m_questTitleHdr->setStyleSheet(QString(
                "color: %1; font-size: 13px; font-weight: 700;"
                " border: none; background: transparent;").arg(Cl::ink3));
        } else if (finished || (activeDay < 0)) {
            m_questDayLbl->setText("QUEST LINE COMPLETE");
            m_questTitleHdr->setText("All days conquered! 🏆");
            m_todayPanel->setStyleSheet(QString(
                "QFrame { background: %1; border: 1px solid %2;"
                " border-left: 4px solid %3; border-radius: 10px; }")
                .arg(Cl::goldBg).arg(Cl::goldBdr).arg(Cl::gold2));
            m_questDayLbl->setStyleSheet(QString(
                "color: %1; font-size: 10px; font-weight: 600;"
                " letter-spacing: 2px; border: none; background: transparent;").arg(Cl::gold2));
            m_questTitleHdr->setStyleSheet(QString(
                "color: %1; font-size: 13px; font-weight: 700;"
                " border: none; background: transparent;").arg(Cl::gold));
        } else if (activeDone) {
            // All available days done — show most-recent, styled as complete
            QString datePart = Util::planDateFor(*plan, activeDay);
            m_questDayLbl->setText(QString("DAY %1 OF %2%3  ·  ALL CAUGHT UP ✓")
                .arg(activeDay).arg(plan->duration)
                .arg(datePart.isEmpty() ? "" : "  ·  " + datePart));
            m_questTitleHdr->setText(q && !q->title.isEmpty() ? q->title : "Daily Quest");
            m_todayPanel->setStyleSheet(QString(
                "QFrame { background: %1; border: 1px solid %2;"
                " border-left: 4px solid %3; border-radius: 10px; }")
                .arg(Cl::greenBg).arg(Cl::greenBdr).arg(Cl::green2));
            m_questDayLbl->setStyleSheet(QString(
                "color: %1; font-size: 10px; font-weight: 600;"
                " letter-spacing: 2px; border: none; background: transparent;").arg(Cl::green2));
            m_questTitleHdr->setStyleSheet(QString(
                "color: %1; font-size: 13px; font-weight: 700;"
                " border: none; background: transparent;").arg(Cl::ink4));
        } else if (isMs) {
            QString datePart = Util::planDateFor(*plan, activeDay);
            m_questDayLbl->setText(QString("DAY %1%2 · 🏆 CHECKPOINT  ·  +%3 XP")
                .arg(activeDay)
                .arg(datePart.isEmpty() ? "" : "  " + datePart)
                .arg(q ? q->xp : 500));
            m_questTitleHdr->setText(q && !q->title.isEmpty() ? q->title : "Milestone");
            m_todayPanel->setStyleSheet(QString(
                "QFrame { background: %1; border: 1px solid %2;"
                " border-left: 4px solid %3; border-radius: 10px; }")
                .arg(Cl::goldBg).arg(Cl::goldBdr).arg(Cl::gold2));
            m_questDayLbl->setStyleSheet(QString(
                "color: %1; font-size: 10px; font-weight: 600;"
                " letter-spacing: 2px; border: none; background: transparent;").arg(Cl::gold2));
            m_questTitleHdr->setStyleSheet(QString(
                "color: %1; font-size: 13px; font-weight: 700;"
                " border: none; background: transparent;").arg(Cl::gold));
        } else {
            QString datePart = Util::planDateFor(*plan, activeDay);
            m_questDayLbl->setText(QString("DAY %1%2  ·  +%3 XP")
                .arg(activeDay)
                .arg(datePart.isEmpty() ? QString("  OF %1").arg(plan->duration) : QString("  OF %1  ·  %2").arg(plan->duration).arg(datePart))
                .arg(q ? q->xp : 100));
            m_questTitleHdr->setText(q && !q->title.isEmpty() ? q->title : "Daily Quest");
            m_todayPanel->setStyleSheet(QString(
                "QFrame { background: %1; border: 1px solid %2;"
                " border-left: 4px solid %3; border-radius: 10px; }")
                .arg(Cl::greenBg).arg(Cl::greenBdr).arg(Cl::green));
            m_questDayLbl->setStyleSheet(QString(
                "color: %1; font-size: 10px; font-weight: 600;"
                " letter-spacing: 2px; border: none; background: transparent;").arg(Cl::green2));
            m_questTitleHdr->setStyleSheet(QString(
                "color: %1; font-size: 13px; font-weight: 700;"
                " border: none; background: transparent;").arg(Cl::ink));
        }

        // ── Mark button: acts on activeDay ─────────────────────────
        if (notStarted || finished || activeDay < 0) {
            m_markBtn->setEnabled(false);
            m_markBtn->setText(notStarted ? "Not started" : "Finished ✓");
            m_markBtn->setStyleSheet(QString(
                "QPushButton { background: %1; color: %2; border: 1px solid %2;"
                " border-radius: 8px; font-size: 12px; font-weight: 700; }")
                .arg(Cl::cardBg2).arg(Cl::ink4));
        } else if (activeDone) {
            m_markBtn->setEnabled(true);
            m_markBtn->setIcon(svgIcon("check", 14, QColor(Cl::ink3)));
        m_markBtn->setText("  Undo");
            m_markBtn->setStyleSheet(QString(
                "QPushButton {"
                "  background: %1; color: %2;"
                "  border: 1px solid %2; border-radius: 8px;"
                "  font-size: 12px; font-weight: 700; letter-spacing: 1px;"
                "  padding: 8px 16px; min-width: 104px;"
                "}"
                "QPushButton:hover { background: #FEF2F2; border-color: %3; color: %3; }")
                .arg(Cl::cardBg2).arg(Cl::ink4).arg(Cl::red));
        } else {
            m_markBtn->setEnabled(true);
            m_markBtn->setIcon(svgIcon("check", 14, QColor(Cl::white_)));
        m_markBtn->setText("  Done");
            m_markBtn->setStyleSheet(QString(
                "QPushButton {"
                "  background: %1; color: #FFFFFF; border: none;"
                "  border-radius: 8px; font-size: 12px; font-weight: 700;"
                "  padding: 8px 16px; min-width: 104px;"
                "}"
                "QPushButton:hover { background: %2; }")
                .arg(Cl::green).arg(Cl::green2));
        }

        // ── Row 4: compact stats ───────────────────────────────────
        int validCompleted = Plan::validCompletedCount(prog.completedDays, plan->duration);
        QString stallStr = prog.stalledDays.isEmpty()
            ? "" : QString("  ·  ⏸%1 stalled").arg(prog.stalledDays.size());
        m_statLbl->setText(QString("%1%  ·  %2/%3 days  ·  streak: %4%5  ·  %6 XP")
            .arg(pct)
            .arg(validCompleted)
            .arg(plan->duration)
            .arg(streak)
            .arg(stallStr)
            .arg(xp));

        // Store activeDay for onMarkToday to use
        m_activeDay = activeDay;

        // Refresh expanded body if open
        if (m_expanded) populateBody(*plan, prog);
    }

signals:
    void dataChanged();

private:
    QString    m_planId;
    bool       m_expanded = false;
    int        m_activeDay = 1;        // next uncompleted day — set by refresh()
    QVBoxLayout* m_root   = nullptr;
    QWidget*   m_headerW  = nullptr;
    QWidget*   m_bodyW    = nullptr;
    QFrame*    m_sepLine  = nullptr;
    // Top row
    QLabel*    m_titleLbl = nullptr;
    QLabel*    m_rankLbl  = nullptr;
    QLabel*    m_arrowLbl = nullptr;
    // XP bar
    XpBarWidget* m_xpBar  = nullptr;
    // Today's quest panel — always visible in header
    QFrame*    m_todayPanel    = nullptr;
    QLabel*    m_questDayLbl   = nullptr;   // "Day 5 of 30 · +120 XP"
    QLabel*    m_questTitleHdr = nullptr;   // the topic name, large
    // Bottom action row
    QLabel*    m_statLbl  = nullptr;
    QPushButton* m_markBtn = nullptr;
    // Expanded body
    QVBoxLayout* m_upcomingLayout = nullptr;

    void buildHeader() {
        m_headerW = new QWidget;
        m_headerW->setStyleSheet("background: transparent; border: none;");
        m_headerW->setCursor(QCursor(Qt::PointingHandCursor));
        auto* hl = new QVBoxLayout(m_headerW);
        hl->setContentsMargins(20, 16, 20, 14); hl->setSpacing(10);

        // ── Row 1: Plan name + rank badge + expand arrow ──────────
        auto* r1 = new QHBoxLayout;
        m_titleLbl = new QLabel;
        m_titleLbl->setStyleSheet(QString(
            "color: %1; font-size: 15px; font-weight: 700;"
            " border: none; background: transparent;").arg(Cl::ink));
        m_rankLbl = new QLabel;
        m_arrowLbl = new QLabel("▶");
        m_arrowLbl->setStyleSheet(QString(
            "color: %1; font-size: 10px; border: none; background: transparent;")
            .arg(Cl::ink4));
        r1->addWidget(m_titleLbl); r1->addStretch();
        r1->addWidget(m_rankLbl); r1->addSpacing(10); r1->addWidget(m_arrowLbl);
        hl->addLayout(r1);

        // ── Row 2: XP progress bar (thin) ─────────────────────────
        m_xpBar = new XpBarWidget;
        m_xpBar->setFixedHeight(5);
        hl->addWidget(m_xpBar);

        // ── Row 3: TODAY'S QUEST PANEL — always visible ───────────
        // This is the main change: quest topic + mark button are
        // front and centre, no expansion needed for daily use.
        m_todayPanel = new QFrame;
        m_todayPanel->setStyleSheet(QString(
            "QFrame { background: %1; border: 1px solid %2;"
            " border-left: 4px solid %3; border-radius: 10px; }")
            .arg(Cl::tealBg).arg(Cl::tealBdr).arg(Cl::teal));

        auto* panelLayout = new QHBoxLayout(m_todayPanel);
        panelLayout->setContentsMargins(14, 10, 14, 10);   // symmetric 14px each side
        panelLayout->setSpacing(12);

        // Left side: day label + topic name
        auto* textCol = new QVBoxLayout;
        textCol->setSpacing(3);
        m_questDayLbl = new QLabel;
        m_questDayLbl->setStyleSheet(QString(
            "color: %1; font-size: 10px; font-weight: 600;"
            " letter-spacing: 2px; border: none; background: transparent;")
            .arg(Cl::green2));
        m_questTitleHdr = new QLabel;
        m_questTitleHdr->setStyleSheet(QString(
            "color: %1; font-size: 13px; font-weight: 700;"
            " border: none; background: transparent;").arg(Cl::ink));
        m_questTitleHdr->setWordWrap(true);          // allow wrapping on narrow windows
        m_questTitleHdr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        textCol->addWidget(m_questDayLbl);
        textCol->addWidget(m_questTitleHdr);
        panelLayout->addLayout(textCol, 1);

        // Right side: Done / Undo button — min width so text never clips
        m_markBtn = new QPushButton(svgIcon("check", 14, QColor(Cl::white_)), "  Done");
        m_markBtn->setProperty("role", "success");
        m_markBtn->setMinimumWidth(104);   // wide enough for "↩  Undo"
        m_markBtn->setFixedHeight(44);
        m_markBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        m_markBtn->setStyleSheet(QString(
            "QPushButton[role='success'] {"
            "  background: %1; color: #FFFFFF; border: none;"
            "  border-radius: 8px; font-size: 12px; font-weight: 700;"
            "  letter-spacing: 1px;"
            "}"
            "QPushButton[role='success']:hover { background: %2; }"
            "QPushButton[role='success']:disabled {"
            "  background: %3; color: %4; border: 1px solid %5;"
            "}")
            .arg(Cl::teal).arg(Cl::teal2)
            .arg(Cl::cardBg2).arg(Cl::ink4).arg(Cl::border));
        connect(m_markBtn, &QPushButton::clicked, this, &QuestCard::onMarkToday);
        panelLayout->addWidget(m_markBtn);

        hl->addWidget(m_todayPanel);

        // ── Row 4: compact stats + Details button ─────────────────
        auto* r4 = new QHBoxLayout;
        m_statLbl = new QLabel;
        m_statLbl->setStyleSheet(QString(
            "color: %1; font-size: 10px; border: none; background: transparent;")
            .arg(Cl::ink4));
        m_statLbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        m_statLbl->setMinimumWidth(0);   // allow shrinking
        auto* detailsBtn = new QPushButton("Details →");
        detailsBtn->setFixedHeight(28);
        detailsBtn->setStyleSheet(QString(
            "QPushButton { background: transparent; border: none;"
            " color: %1; font-size: 11px; font-weight: 600; padding: 0 4px; }"
            "QPushButton:hover { color: %2; }")
            .arg(Cl::ink4).arg(Cl::blue));
        connect(detailsBtn, &QPushButton::clicked, this, &QuestCard::onDetails);
        r4->addWidget(m_statLbl); r4->addStretch(); r4->addWidget(detailsBtn);
        hl->addLayout(r4);

        m_root->addWidget(m_headerW);
        m_headerW->installEventFilter(this);
    }

    bool eventFilter(QObject* obj, QEvent* ev) override {
        if (obj == m_headerW && ev->type() == QEvent::MouseButtonPress) {
            auto* me = static_cast<QMouseEvent*>(ev);
            if (me->button() == Qt::LeftButton) {
                // Don't expand if clicking on buttons
                QWidget* child = m_headerW->childAt(me->pos());
                if (child && qobject_cast<QPushButton*>(child)) return false;
                toggleExpand();
                return true;
            }
        }
        return QFrame::eventFilter(obj, ev);
    }

    void buildBody() {
        m_sepLine = new QFrame;
        m_sepLine->setFrameShape(QFrame::HLine);
        m_sepLine->setStyleSheet(QString(
            "background: %1; max-height: 1px; border: none;").arg(Cl::border));
        m_sepLine->setVisible(false);
        m_root->addWidget(m_sepLine);

        m_bodyW = new QWidget;
        m_bodyW->setStyleSheet("background: transparent; border: none;");
        m_bodyW->setVisible(false);
        auto* bl = new QVBoxLayout(m_bodyW);
        bl->setContentsMargins(22, 10, 22, 16); bl->setSpacing(4);

        auto* upHdr = new QLabel("COMING UP");
        upHdr->setStyleSheet(QString(
            "color: %1; font-size: 10px; font-weight: 700; letter-spacing: 4px;"
            " border: none; background: transparent;").arg(Cl::ink4));
        bl->addWidget(upHdr);

        auto* upContainer = new QWidget;
        upContainer->setStyleSheet("background: transparent; border: none;");
        m_upcomingLayout = new QVBoxLayout(upContainer);
        m_upcomingLayout->setContentsMargins(0,4,0,0);
        m_upcomingLayout->setSpacing(2);
        bl->addWidget(upContainer);

        m_root->addWidget(m_bodyW);
    }

    void populateBody(const Plan& plan, const Progress& prog) {
        int activeDay  = Util::nextPendingDay(plan, prog);
        bool activeDone = activeDay > 0 && prog.completedDays.contains(activeDay);

        // Clear and repopulate coming-up list only
        QLayoutItem* item;
        while ((item = m_upcomingLayout->takeAt(0)) != nullptr) {
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }

        int shown = 0;
        int startFrom = activeDay + 1;   // always start after the active quest
        for (int d = startFrom; d <= plan.duration && shown < 5; d++) {
            if (prog.completedDays.contains(d) || prog.stalledDays.contains(d)) continue;
            const Quest* uq = plan.questForDay(d);
            QString dateStr = Util::planDateFor(plan, d);
            auto* row = new QWidget;
            row->setStyleSheet("background: transparent; border: none;");
            auto* rl = new QHBoxLayout(row);
            rl->setContentsMargins(0, 2, 0, 2);
            // Day number + calendar date
            auto* dL = new QLabel(dateStr.isEmpty()
                ? QString("Day %1").arg(d)
                : QString("Day %1  ·  %2").arg(d).arg(dateStr));
            dL->setStyleSheet(QString(
                "color: %1; font-size: 11px; min-width: 110px;"
                " border: none; background: transparent;").arg(Cl::ink4));
            auto* tL = new QLabel(uq && !uq->title.isEmpty() ? uq->title : "—");
            tL->setStyleSheet(QString(
                "color: %1; font-size: 12px; border: none; background: transparent;")
                .arg(uq && uq->isMilestone ? Cl::gold : Cl::ink3));
            auto* xL = new QLabel(QString("+%1 xp").arg(uq ? uq->xp : 100));
            xL->setStyleSheet(QString(
                "color: %1; font-size: 10px; border: none; background: transparent;")
                .arg(Cl::ink4));
            rl->addWidget(dL); rl->addWidget(tL); rl->addStretch(); rl->addWidget(xL);
            m_upcomingLayout->addWidget(row);
            shown++;
        }

        if (shown == 0) {
            auto* done = new QLabel(activeDone
                ? "All available days complete 🎉"
                : "Nothing scheduled ahead — add quests in Setup.");
            done->setStyleSheet(QString(
                "color: %1; font-size: 12px; font-style: italic;"
                " border: none; background: transparent;").arg(Cl::green2));
            m_upcomingLayout->addWidget(done);
        }
    }

    void toggleExpand() {
        m_expanded = !m_expanded;
        m_bodyW->setVisible(m_expanded);
        m_sepLine->setVisible(m_expanded);
        m_arrowLbl->setText(m_expanded ? "▼" : "▶");
        if (m_expanded) {
            auto* plan = DataManager::instance().planById(m_planId);
            auto& prog = DataManager::instance().progressFor(m_planId);
            if (plan) populateBody(*plan, prog);
        }
    }

    void onMarkToday() {
        auto& dm   = DataManager::instance();
        auto* plan = dm.planById(m_planId);
        if (!plan) return;
        auto& prog   = dm.progressFor(m_planId);

        // Use m_activeDay (set during refresh) — the next uncompleted quest
        int day = m_activeDay;
        if (day < 1 || day > plan->duration) return;

        bool alreadyDone = prog.completedDays.contains(day);

        if (alreadyDone) {
            // ── Undo path ───────────────────────────────────────────
            const Quest* q = plan->questForDay(day);
            int xp = q ? q->xp : 100;
            auto* dlg = new QMessageBox(this);
            dlg->setWindowTitle("Undo Completion?");
            dlg->setText(QString(
                "Day %1%2 is already marked complete (+%3 XP).\n\n"
                "Do you want to undo this?")
                .arg(day)
                .arg((q && !q->title.isEmpty()) ? QString("\n\"%1\"").arg(q->title) : "")
                .arg(xp));
            dlg->setIcon(QMessageBox::Question);
            auto* undoBtn = dlg->addButton("↩  Yes, Undo", QMessageBox::DestructiveRole);
            dlg->addButton("Keep It", QMessageBox::RejectRole);
            dlg->exec();
            if (dlg->clickedButton() == undoBtn) {
                dm.unmarkDay(m_planId, day);
                emit dataChanged();
            }
            return;
        }

        // ── Normal mark-complete path ───────────────────────────────
        const Quest* q = plan->questForDay(day);
        int xp = q ? q->xp : 100;
        auto prevRank = Util::currentRank(prog.totalXp, *plan);
        int awarded = dm.markDay(m_planId, day);
        if (awarded == 0) return;

        auto newRank = Util::currentRank(prog.totalXp, *plan);
        bool rankUp  = newRank.rank != prevRank.rank;

        QString msg;
        if (q && q->isMilestone)
            msg = QString("🏆  CHECKPOINT CLEARED\n%1\n+%2 XP")
                .arg(q->milestoneTitle.isEmpty() ? q->title : q->milestoneTitle).arg(xp);
        else if (rankUp)
            msg = QString("⚔️  RANK UP!\n%1-Rank · %2\n+%3 XP\n%4")
                .arg(newRank.rank).arg(newRank.label).arg(xp)
                .arg(Util::RANK_MSG.value(newRank.rank));
        else
            msg = QString("✅  Quest Complete — Day %1\n+%2 XP").arg(day).arg(xp);

        QMessageBox::information(this, "Quest Complete", msg);
        emit dataChanged();
    }

    void onDetails() {
        DetailsDialog dlg(m_planId, this);
        connect(&dlg, &DetailsDialog::dataChanged, this, &QuestCard::dataChanged);
        dlg.exec();
        emit dataChanged();
    }
};

// ============================================================
//  DASHBOARD TAB  — Pending / Completed sections
// ============================================================
class DashboardTab : public QWidget {
    Q_OBJECT
public:
    explicit DashboardTab(QWidget* parent = nullptr) : QWidget(parent) {
        auto* root = new QVBoxLayout(this);
        root->setContentsMargins(0,0,0,0); root->setSpacing(0);

        // ── Header bar ────────────────────────────────────────────
        auto* header = new QWidget;
        header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        header->setStyleSheet(QString(
            "background: %1; border-bottom: 1px solid %2;")
            .arg(Cl::white_).arg(Cl::border));
        auto* hl = new QHBoxLayout(header);
        hl->setContentsMargins(24, 12, 24, 12);
        auto* title = new QLabel("reSTEM");
        title->setStyleSheet(QString(
            "font-size: 18px; font-weight: 700; letter-spacing: 4px;"
            " color: %1; border: none;").arg(Cl::navy));
        auto* newBtn = new QPushButton("+ New Quest Line");
        newBtn->setProperty("role", "primary");
        newBtn->setFixedHeight(34);
        connect(newBtn, &QPushButton::clicked,
                this, &DashboardTab::newQuestLineRequested);
        hl->addWidget(title); hl->addStretch(); hl->addWidget(newBtn);
        root->addWidget(header);

        // ── Scrollable content ────────────────────────────────────
        m_scrollContent = new QWidget;
        m_scrollContent->setStyleSheet(
            QString("background: %1; border: none;").arg(Cl::cream));
        m_outerVl = new QVBoxLayout(m_scrollContent);
        m_outerVl->setContentsMargins(24, 20, 24, 20);
        m_outerVl->setSpacing(0);

        // ── Pending section ───────────────────────────────────────
        m_pendingHdr = makeSectionHeader("  IN PROGRESS", Cl::blue);
        m_pendingContainer = new QWidget;
        m_pendingContainer->setStyleSheet("background: transparent; border: none;");
        m_pendingVl = new QVBoxLayout(m_pendingContainer);
        m_pendingVl->setContentsMargins(0,0,0,0); m_pendingVl->setSpacing(14);

        m_emptyPending = new QLabel(
            "No active quest lines.\nCreate one with  + New Quest Line  above.");
        m_emptyPending->setAlignment(Qt::AlignCenter);
        m_emptyPending->setStyleSheet(QString(
            "color: %1; font-size: 13px; border: 1px dashed %2;"
            " border-radius: 12px; padding: 32px; background: %3;")
            .arg(Cl::ink4).arg(Cl::border).arg(Cl::cardBg2));
        m_pendingVl->addWidget(m_emptyPending);

        // ── Completed section ─────────────────────────────────────
        m_completedHdr = makeSectionHeader("  COMPLETED", Cl::green2);
        m_completedContainer = new QWidget;
        m_completedContainer->setStyleSheet("background: transparent; border: none;");
        m_completedVl = new QVBoxLayout(m_completedContainer);
        m_completedVl->setContentsMargins(0,0,0,0); m_completedVl->setSpacing(14);

        m_outerVl->addWidget(m_pendingHdr);
        m_outerVl->addSpacing(10);
        m_outerVl->addWidget(m_pendingContainer);
        m_outerVl->addSpacing(28);
        m_outerVl->addWidget(m_completedHdr);
        m_outerVl->addSpacing(10);
        m_outerVl->addWidget(m_completedContainer);
        m_outerVl->addStretch();

        auto* scroll = new QScrollArea;
        scroll->setWidgetResizable(true);
        scroll->setWidget(m_scrollContent);
        root->addWidget(scroll);
    }

    void refresh() {
        auto& dm = DataManager::instance();

        // Collect live plan ids (non-archived)
        QSet<QString> activePlanIds;
        for (const auto& p : dm.plans)
            if (!p.archived) activePlanIds.insert(p.id);

        // Remove cards for deleted or newly-archived plans
        for (auto it = m_cards.begin(); it != m_cards.end(); ) {
            if (!activePlanIds.contains(it.key())) {
                m_completedSection.remove(it.key());
                it.value()->deleteLater();
                it = m_cards.erase(it);
            } else ++it;
        }

        int pendingCount   = 0;
        int completedCount = 0;

        for (const auto& plan : dm.plans) {
            if (plan.archived) continue;

            const Progress& prog = dm.progressFor(plan.id);
            bool complete = plan.isCompleteWithProgress(prog.completedDays);

            // Create card if new — place directly into correct section
            if (!m_cards.contains(plan.id)) {
                auto* card = new QuestCard(plan.id);
                connect(card, &QuestCard::dataChanged,
                        this, &DashboardTab::refresh);
                m_cards[plan.id] = card;
                if (complete) {
                    m_completedVl->addWidget(card);
                    m_completedSection.insert(plan.id);
                } else {
                    m_pendingVl->addWidget(card);
                }
            }

            QuestCard* card = m_cards[plan.id];
            bool shouldBeComplete = complete;
            bool isInComplete = m_completedSection.contains(plan.id);

            // Only re-parent when the section genuinely changes
            if (shouldBeComplete != isInComplete) {
                card->setParent(nullptr);
                if (shouldBeComplete) {
                    m_completedVl->addWidget(card);
                    m_completedSection.insert(plan.id);
                } else {
                    m_pendingVl->addWidget(card);
                    m_completedSection.remove(plan.id);
                }
            }
            card->setVisible(true);
            card->refresh();

            if (complete) completedCount++;
            else          pendingCount++;
        }

        // Empty state for pending section
        m_emptyPending->setVisible(pendingCount == 0);

        // Hide completed section header when nothing there
        m_completedHdr->setVisible(completedCount > 0);
        m_completedContainer->setVisible(completedCount > 0);
    }

signals:
    void newQuestLineRequested();

private:
    QWidget*                m_scrollContent     = nullptr;
    QVBoxLayout*            m_outerVl           = nullptr;
    QWidget*                m_pendingHdr        = nullptr;
    QWidget*                m_completedHdr      = nullptr;
    QWidget*                m_pendingContainer  = nullptr;
    QWidget*                m_completedContainer= nullptr;
    QVBoxLayout*            m_pendingVl         = nullptr;
    QVBoxLayout*            m_completedVl       = nullptr;
    QLabel*                 m_emptyPending      = nullptr;
    QMap<QString,QuestCard*> m_cards;
    QSet<QString>            m_completedSection;  // tracks which plan IDs are in completed section

    static QWidget* makeSectionHeader(const QString& text, const QString& accentColor) {
        auto* w = new QWidget;
        w->setFixedHeight(32);
        w->setStyleSheet("background: transparent; border: none;");
        auto* hl = new QHBoxLayout(w);
        hl->setContentsMargins(0, 0, 0, 0); hl->setSpacing(10);

        auto* dot = new QLabel;
        dot->setFixedSize(8, 8);
        dot->setStyleSheet(QString(
            "background: %1; border-radius: 4px; border: none;")
            .arg(accentColor));

        auto* lbl = new QLabel(text);
        lbl->setStyleSheet(QString(
            "color: %1; font-size: 11px; font-weight: 700;"
            " letter-spacing: 3px; border: none; background: transparent;")
            .arg(accentColor));

        auto* line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setStyleSheet(QString(
            "background: %1; max-height: 1px; border: none;")
            .arg(accentColor));
        line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        hl->addWidget(dot);
        hl->addWidget(lbl);
        hl->addWidget(line, 1);
        return w;
    }
};

// ============================================================
//  SETUP TAB
// ============================================================
class SetupTab : public QWidget {
    Q_OBJECT
public:
    explicit SetupTab(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        auto* root = new QHBoxLayout(this);
        root->setContentsMargins(0,0,0,0); root->setSpacing(0);
        root->addWidget(buildLeft());
        root->addWidget(buildRight(), 1);
    }

    void refresh() {
        m_block = true;
        m_planCombo->clear();
        for (const auto& p : DataManager::instance().plans)
            if (!p.archived)
                m_planCombo->addItem(p.name, p.id);

        // Determine which plan to show — prefer activePlanId, fall back to first
        QString targetId = DataManager::instance().primaryActivePlanId();
        int idx = m_planCombo->findData(targetId);
        if (idx < 0 && m_planCombo->count() > 0) {
            idx = 0;
            targetId = m_planCombo->itemData(0).toString();
        }
        if (idx >= 0) m_planCombo->setCurrentIndex(idx);

        // CRITICAL: m_planId must be set here because onPlanSelected()
        // is blocked by m_block=true during combo population
        m_planId = targetId;

        m_block = false;
        loadCurrentPlan();
    }

signals:
    void planChanged();

private:
    bool          m_block     = false;
    QString       m_planId;
    QComboBox*    m_planCombo = nullptr;
    QLineEdit*    m_nameEdit  = nullptr;
    QTextEdit*    m_descEdit  = nullptr;
    QDateEdit*    m_startDate = nullptr;
    QSpinBox*     m_durSpin   = nullptr;
    QTableWidget* m_table     = nullptr;
    // Rank spins
    QMap<QString, QSpinBox*> m_rankSpins;
    QSpinBox*     m_sxpSpin   = nullptr;
    QSlider*      m_expSlider = nullptr;
    QLabel*       m_expLbl    = nullptr;
    QLabel*       m_curvePrev = nullptr;
    QTextEdit*    m_importText = nullptr;

    // ── Left panel ────────────────────────────────────
    QWidget* buildLeft() {
        auto* w = new QWidget;
        w->setMinimumWidth(220);
        w->setMaximumWidth(320);
        w->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        w->setStyleSheet(QString("background: %1; border-right: 1px solid %2;").arg(Cl::cardBg2).arg(Cl::border));
        auto* vl = new QVBoxLayout(w);
        vl->setContentsMargins(16,16,16,16); vl->setSpacing(10);

        auto* sl1 = new QLabel("QUEST LINES");
        sl1->setStyleSheet(QString("color: %1; font-size: 10px; font-weight: 700; letter-spacing: 3px;").arg(Cl::ink4));
        vl->addWidget(sl1);

        m_planCombo = new QComboBox;
        connect(m_planCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this](int) { if (!m_block) onPlanSelected(); });
        vl->addWidget(m_planCombo);

        auto* newBtn = new QPushButton("+ Create New Quest Line");
        newBtn->setProperty("role", "primary");
        connect(newBtn, &QPushButton::clicked, this, &SetupTab::newPlanDialog);
        vl->addWidget(newBtn);

        vl->addSpacing(8);
        auto* sl2 = new QLabel("PLAN INFO");
        sl2->setStyleSheet(sl1->styleSheet());
        vl->addWidget(sl2);

        auto* nl = new QLabel("Name");
        nl->setStyleSheet("background: transparent; border: none;");
        m_nameEdit = new QLineEdit;
        m_nameEdit->setPlaceholderText("Quest line name");
        connect(m_nameEdit, &QLineEdit::textChanged, this, [this](const QString& t) {
            savePlanField("name", t);
        });
        vl->addWidget(nl); vl->addWidget(m_nameEdit);

        auto* dl = new QLabel("Description");
        dl->setStyleSheet("background: transparent; border: none;");
        m_descEdit = new QTextEdit;
        m_descEdit->setPlaceholderText("Optional");
        m_descEdit->setFixedHeight(68);
        connect(m_descEdit, &QTextEdit::textChanged, this, [this]() {
            savePlanField("description", m_descEdit->toPlainText());
        });
        vl->addWidget(dl); vl->addWidget(m_descEdit);

        auto* dateRow = new QHBoxLayout;
        auto* sc = new QVBoxLayout;
        sc->addWidget([]{ auto* l = new QLabel("Start Date"); l->setStyleSheet("background:transparent;border:none;"); return l; }());
        m_startDate = new QDateEdit(QDate::currentDate());
        m_startDate->setCalendarPopup(true);
        connect(m_startDate, &QDateEdit::dateChanged, this, [this](const QDate& d) {
            savePlanField("start_date", d.toString("yyyy-MM-dd"));
        });
        sc->addWidget(m_startDate);
        auto* dc = new QVBoxLayout;
        dc->addWidget([]{ auto* l = new QLabel("Duration"); l->setStyleSheet("background:transparent;border:none;"); return l; }());
        m_durSpin = new QSpinBox;
        m_durSpin->setRange(1, 3650); m_durSpin->setValue(120); m_durSpin->setSuffix(" days");
        connect(m_durSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int v) {
            savePlanField("duration", v);
        });
        dc->addWidget(m_durSpin);
        dateRow->addLayout(sc); dateRow->addLayout(dc);
        vl->addLayout(dateRow);

        vl->addStretch();

        auto* delBtn = new QPushButton("Delete Quest Line");
        delBtn->setProperty("role", "danger");
        connect(delBtn, &QPushButton::clicked, this, &SetupTab::deletePlan);
        vl->addWidget(delBtn);
        return w;
    }

    // ── Right panel ──────────────────────────────────
    QWidget* buildRight() {
        auto* tabs = new QTabWidget;
        tabs->setStyleSheet("QTabBar::tab { padding: 9px 18px; }");
        tabs->addTab(buildCurriculumTab(), " Curriculum");
        tabs->addTab(buildRankTab(),       " Rank XP");
        tabs->addTab(buildImportTab(),     " Bulk Import");
        tabs->setTabIcon(0, svgIcon("list",         16, QColor(Cl::blue)));
        tabs->setTabIcon(1, svgIcon("rank",         16, QColor(Cl::blue)));
        tabs->setTabIcon(2, svgIcon("achievements", 16, QColor(Cl::blue)));
        tabs->setIconSize(QSize(16, 16));
        return tabs;
    }

    // ── Curriculum table ─────────────────────────────
    QWidget* buildCurriculumTab() {
        auto* w  = new QWidget;
        auto* vl = new QVBoxLayout(w);
        vl->setContentsMargins(18,14,18,14); vl->setSpacing(10);

        auto* tip = new QLabel("💡  Click topic cells to edit. Toggle ☑ for checkpoints. XP column is directly editable. Green = already completed.");
        tip->setWordWrap(true);
        tip->setStyleSheet(QString("color: %1; font-size: 11px; background: %2; border: 1px solid %3; border-left: 3px solid %4; border-radius: 8px; padding: 8px 12px;").arg(Cl::ink2).arg(Cl::blueBg).arg(Cl::blueBdr).arg(Cl::blue));
        vl->addWidget(tip);

        m_table = new QTableWidget(0, 6);
        m_table->setHorizontalHeaderLabels({"DAY", "DATE", "TOPIC / QUEST TITLE", "MILESTONE", "XP", "STATUS"});
        m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
        m_table->setColumnWidth(0, 48);
        m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
        m_table->setColumnWidth(1, 74);
        m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
        m_table->setColumnWidth(3, 96);
        m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
        m_table->setColumnWidth(4, 92);   // wide enough for 4-digit XP + spinbox arrows
        m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
        m_table->setColumnWidth(5, 78);
        m_table->verticalHeader()->setVisible(false);
        m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_table->setAlternatingRowColors(true);
        m_table->setStyleSheet(m_table->styleSheet() +
            QString("QTableWidget { alternate-background-color: %1; }").arg(Cl::cardBg2));
        connect(m_table, &QTableWidget::cellChanged, this, &SetupTab::onCellChanged);
        vl->addWidget(m_table);
        return w;
    }

    void populateTable() {
        auto* plan = DataManager::instance().planById(m_planId);
        if (!plan) { m_table->setRowCount(0); return; }

        m_block = true;
        disconnect(m_table, &QTableWidget::cellChanged, this, &SetupTab::onCellChanged);

        auto& prog    = DataManager::instance().progressFor(m_planId);
        int   dur     = plan->duration;
        m_table->setRowCount(dur);

        for (int d = 1; d <= dur; d++) {
            int  row  = d - 1;
            const Quest* q = plan->questForDay(d);
            bool done     = prog.completedDays.contains(d);
            bool isMs     = q && q->isMilestone;

            // Col 0: Day number
            auto* di = new QTableWidgetItem(QString::number(d));
            di->setFlags(Qt::ItemIsEnabled);
            di->setTextAlignment(Qt::AlignCenter);
            if (isMs) di->setForeground(QColor(Cl::gold));
            m_table->setItem(row, 0, di);

            // Col 1: Calendar date
            QString dateStr = Util::planDateFor(*plan, d);
            auto* dateItem = new QTableWidgetItem(dateStr);
            dateItem->setFlags(Qt::ItemIsEnabled);
            dateItem->setTextAlignment(Qt::AlignCenter);
            dateItem->setForeground(QColor(done ? Cl::green2 : Cl::ink4));
            m_table->setItem(row, 1, dateItem);

            // Col 2: Topic
            auto* ti = new QTableWidgetItem(q ? q->title : "");
            if (done) {
                ti->setFlags(Qt::ItemIsEnabled);
                ti->setForeground(QColor(Cl::ink4));
            } else {
                ti->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
            }
            if (isMs && !done) ti->setForeground(QColor(Cl::gold));
            m_table->setItem(row, 2, ti);

            // Col 3: Milestone checkbox
            auto* cb = new QCheckBox;
            cb->setChecked(isMs);
            cb->setEnabled(!done);
            cb->setStyleSheet("QCheckBox { background: transparent; padding-left: 24px; }");
            connect(cb, &QCheckBox::toggled, this, [this, d](bool checked) {
                if (m_block) return;
                onMilestoneChanged(d, checked);
            });
            auto* cbc = new QWidget;
            cbc->setStyleSheet("background: transparent;");
            auto* cbl = new QHBoxLayout(cbc);
            cbl->setContentsMargins(0,0,0,0);
            cbl->setAlignment(Qt::AlignCenter);
            cbl->addWidget(cb);
            m_table->setCellWidget(row, 3, cbc);

            // Col 4: XP spinbox
            auto* xs = new QSpinBox;
            xs->setRange(1, 9999);
            xs->setValue(q ? q->xp : 100);
            xs->setEnabled(!done);
            xs->setStyleSheet(QString("QSpinBox { border: none; background: transparent; color: %1; font-weight: 600; }")
                .arg(done ? Cl::ink4 : (isMs ? Cl::gold2 : Cl::blue)));
            connect(xs, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, d](int v) {
                if (m_block) return;
                onXpChanged(d, v);
            });
            m_table->setCellWidget(row, 4, xs);

            // Col 5: Status
            QString statusText = done ? "✓ Done" : (isMs ? "🏆 MS" : "—");
            auto* si = new QTableWidgetItem(statusText);
            si->setFlags(Qt::ItemIsEnabled);
            si->setTextAlignment(Qt::AlignCenter);
            si->setForeground(QColor(done ? Cl::green : (isMs ? Cl::gold2 : Cl::ink4)));
            m_table->setItem(row, 5, si);
            m_table->setRowHeight(row, 36);
        }

        connect(m_table, &QTableWidget::cellChanged, this, &SetupTab::onCellChanged);
        m_block = false;
    }

    void onCellChanged(int row, int col) {
        if (m_block || col != 2) return;  // col 2 = topic title
        auto* plan = DataManager::instance().planById(m_planId);
        if (!plan) return;
        int day = row + 1;
        auto* item = m_table->item(row, col);
        if (!item) return;
        plan->ensureQuest(day);
        auto* q = plan->questForDay(day);
        if (q) q->title = item->text().trimmed();
        DataManager::instance().save();
    }

    void onMilestoneChanged(int day, bool ms) {
        auto* plan = DataManager::instance().planById(m_planId);
        if (!plan) return;
        plan->ensureQuest(day);
        auto* q = plan->questForDay(day);
        if (!q) return;
        q->isMilestone = ms;
        if (ms) {
            if (q->xp <= 100) q->xp = 500;
            if (q->milestoneTitle.isEmpty()) q->milestoneTitle = q->title;
        } else {
            if (q->xp == 500) q->xp = 100;
        }
        DataManager::instance().save();
        populateTable();
    }

    void onXpChanged(int day, int value) {
        auto* plan = DataManager::instance().planById(m_planId);
        if (!plan) return;
        plan->ensureQuest(day);
        auto* q = plan->questForDay(day);
        if (q) q->xp = std::clamp(value, 1, 9999);
        DataManager::instance().save();
        // If this day was already completed, recalculate total XP
        recalcXpAfterEdit(day);
    }

    // ── Rank threshold tab ────────────────────────────
    QWidget* buildRankTab() {
        auto* w      = new QWidget;
        auto* scroll = new QScrollArea;
        scroll->setWidgetResizable(true); scroll->setWidget(w);
        auto* vl = new QVBoxLayout(w);
        vl->setContentsMargins(20,16,20,16); vl->setSpacing(14);

        auto* tip = new QLabel("Set the XP required for each rank. E-Rank is always 0. Each subsequent rank must be higher.");
        tip->setWordWrap(true);
        tip->setStyleSheet(QString("color: %1; font-size: 11px; background: %2; border: 1px solid %3; border-left: 3px solid %4; border-radius: 8px; padding: 8px 12px;").arg(Cl::ink2).arg(Cl::blueBg).arg(Cl::blueBdr).arg(Cl::blue));
        vl->addWidget(tip);

        // Rank inputs
        auto* tGrp  = new QGroupBox("RANK XP THRESHOLDS");
        auto* tGrid = new QGridLayout(tGrp);
        tGrid->setSpacing(10);

        for (int i = 0; i < Util::RANK_META.size(); i++) {
            const auto& meta = Util::RANK_META[i];
            auto* col = new QVBoxLayout;

            auto* badge = new QLabel(meta.rank);
            badge->setStyleSheet(QString("color: %1; font-size: 13px; font-weight: 700;"
                " letter-spacing: 2px; border: 2px solid %1; border-radius: 6px; padding: 4px 10px;"
                " background: %2;").arg(meta.color).arg(Cl::white_));
            badge->setAlignment(Qt::AlignCenter);
            auto* lbl = new QLabel(meta.label);
            lbl->setStyleSheet(QString("color: %1; font-size: 10px; border: none;").arg(Cl::ink3));
            lbl->setAlignment(Qt::AlignCenter);
            col->addWidget(badge); col->addWidget(lbl);

            if (meta.rank == "E") {
                auto* fixedLbl = new QLabel("0 XP\n(fixed)");
                fixedLbl->setAlignment(Qt::AlignCenter);
                fixedLbl->setStyleSheet(QString("color: %1; font-size: 12px; border: none;").arg(Cl::ink4));
                col->addWidget(fixedLbl);
            } else {
                auto* spin = new QSpinBox;
                spin->setRange(1, 9'999'999); spin->setSingleStep(500); spin->setSuffix(" XP");
                spin->setStyleSheet(QString("QSpinBox { color: %1; font-weight: 600; }").arg(meta.color));
                connect(spin, QOverload<int>::of(&QSpinBox::valueChanged), this,
                    [this, rank = meta.rank](int v) {
                        if (m_block) return;
                        auto* p = DataManager::instance().planById(m_planId);
                        if (!p) return;
                        p->rankThresholds.setRank(rank, v);
                        p->thresholdsManuallySet = true; // mark as user-customised
                        DataManager::instance().save();
                    });
                m_rankSpins[meta.rank] = spin;
                col->addWidget(spin);
            }
            tGrid->addLayout(col, 0, i);
        }
        vl->addWidget(tGrp);

        // Power curve
        auto* pcGrp = new QGroupBox("POWER CURVE GENERATOR");
        auto* pcVl  = new QVBoxLayout(pcGrp);

        auto* pcTip = new QLabel(
            "Generate thresholds on an exponential curve — exactly like RPG games. "
            "Set the total XP for S-Rank, then drag the exponent:\n"
            "• 1.5 = gentle  |  2.0 = standard  |  3.0 = hard  |  4.0+ = brutal");
        pcTip->setWordWrap(true);
        pcTip->setStyleSheet(QString("color: %1; font-size: 11px; border: none;").arg(Cl::ink3));
        pcVl->addWidget(pcTip);

        auto* paramsRow = new QHBoxLayout;
        auto* sxpCol = new QVBoxLayout;
        sxpCol->addWidget([]{ auto* l = new QLabel("S-Rank XP (max)"); l->setStyleSheet("border:none;"); return l; }());
        m_sxpSpin = new QSpinBox;
        m_sxpSpin->setRange(100, 9'999'999); m_sxpSpin->setSingleStep(1000);
        m_sxpSpin->setValue(10000); m_sxpSpin->setSuffix(" XP");
        connect(m_sxpSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &SetupTab::updateCurvePreview);
        sxpCol->addWidget(m_sxpSpin);
        paramsRow->addLayout(sxpCol); paramsRow->addSpacing(20);

        auto* expCol = new QVBoxLayout;
        m_expLbl = new QLabel("Exponent: 2.0");
        m_expLbl->setStyleSheet(QString("color: %1; font-weight: 600; border: none;").arg(Cl::blue));
        m_expSlider = new QSlider(Qt::Horizontal);
        m_expSlider->setRange(10, 60); m_expSlider->setValue(20); m_expSlider->setTickInterval(5);
        connect(m_expSlider, &QSlider::valueChanged, this, &SetupTab::updateCurvePreview);
        expCol->addWidget(m_expLbl); expCol->addWidget(m_expSlider);
        paramsRow->addLayout(expCol);
        pcVl->addLayout(paramsRow);

        m_curvePrev = new QLabel;
        m_curvePrev->setStyleSheet(QString("background: %1; border: 1px solid %2; border-radius: 8px;"
            " padding: 10px 14px; color: %3; font-size: 11px;").arg(Cl::cardBg2).arg(Cl::border).arg(Cl::ink3));
        m_curvePrev->setWordWrap(true);
        pcVl->addWidget(m_curvePrev);

        auto* applyBtn = new QPushButton("Apply Curve to Current Plan");
        applyBtn->setProperty("role", "gold");
        connect(applyBtn, &QPushButton::clicked, this, &SetupTab::applyCurve);
        pcVl->addWidget(applyBtn);
        vl->addWidget(pcGrp);
        vl->addStretch();

        auto* outer = new QWidget; auto* ol = new QVBoxLayout(outer);
        ol->setContentsMargins(0,0,0,0); ol->addWidget(scroll);
        return outer;
    }

    void updateCurvePreview() {
        if (!m_sxpSpin || !m_expSlider || !m_curvePrev || !m_expLbl) return;
        double exp = m_expSlider->value() / 10.0;
        int    sxp = m_sxpSpin->value();
        m_expLbl->setText(QString("Exponent: %1").arg(exp, 0, 'f', 1));
        auto t = Util::powerCurve(sxp, exp);

        // Live earnable XP check
        auto* plan = DataManager::instance().planById(m_planId);
        int earnableXP = plan ? Util::totalPlanXP(*plan) : 0;

        QStringList lines;
        for (const auto& meta : Util::RANK_META) {
            lines << QString("%1-Rank (%2): %3 XP")
                         .arg(meta.rank).arg(meta.label)
                         .arg(t.forRank(meta.rank));
        }
        if (earnableXP > 0 && sxp > earnableXP) {
            lines << QString("\n⚠  Plan only yields %1 XP total.\n"
                             "   S-Rank is unreachable — consider setting S-Rank to %1 XP.")
                         .arg(earnableXP);
        }
        m_curvePrev->setText(lines.join("\n"));
    }

    void applyCurve() {
        auto* plan = DataManager::instance().planById(m_planId);
        if (!plan) { QMessageBox::warning(this, "No Plan", "Select a plan first."); return; }
        double exp = m_expSlider->value() / 10.0;
        int requestedS = m_sxpSpin->value();
        int earnableXP = Util::totalPlanXP(*plan);

        // Warn if S-rank exceeds what the plan can actually yield
        if (requestedS > earnableXP) {
            auto reply = QMessageBox::question(this, "S-Rank Unreachable",
                QString("The plan only yields %1 XP total, but you've set S-Rank at %2 XP.\n\n"
                        "This means completing 100% of the plan will NOT reach S-Rank.\n\n"
                        "Would you like to cap S-Rank at %1 XP (= 100% completion) instead?")
                    .arg(earnableXP).arg(requestedS),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            if (reply == QMessageBox::Yes) requestedS = earnableXP;
        }

        plan->rankThresholds = Util::powerCurve(requestedS, exp);
        plan->thresholdsManuallySet = false; // power curve = auto, not manual
        DataManager::instance().save();
        loadRankSpins(*plan);
        QMessageBox::information(this, "Applied",
            QString("Power curve (S=%1 XP, exp %2) applied to %3.")
                .arg(requestedS).arg(exp, 0, 'f', 1).arg(plan->name));
    }

    void loadRankSpins(const Plan& plan) {
        m_block = true;
        for (auto it = m_rankSpins.begin(); it != m_rankSpins.end(); ++it)
            it.value()->setValue(plan.rankThresholds.forRank(it.key()));
        m_block = false;
    }

    // ── Bulk import tab ──────────────────────────────
    QWidget* buildImportTab() {
        auto* w  = new QWidget;
        auto* vl = new QVBoxLayout(w);
        vl->setContentsMargins(20,16,20,16); vl->setSpacing(12);

        auto* tip = new QLabel(
            "Paste one topic per line — fills Day 1, 2, 3... in order.\n"
            "• Lines starting with ★ become milestone checkpoints (500 XP by default).\n"
            "• Append |200 to set custom XP:  Pointers and References|200\n"
            "• Example:  ★ CHECKPOINT: Core Done|500");
        tip->setWordWrap(true);
        tip->setStyleSheet(QString("color: %1; font-size: 11px; background: %2; border: 1px solid %3; border-left: 3px solid %4; border-radius: 8px; padding: 10px 12px;").arg(Cl::ink2).arg(Cl::blueBg).arg(Cl::blueBdr).arg(Cl::blue));
        vl->addWidget(tip);

        m_importText = new QTextEdit;
        m_importText->setPlaceholderText(
            "Variables and Data Types\n"
            "Control Flow\n"
            "Loops — for, while, do-while|80\n"
            "★ CHECKPOINT: Basics Complete|500\n"
            "Functions and Scope|150\n"
            "Pointers and References|200\n"
            "★ CHECKPOINT: Core Language Done|500\n..."
        );
        vl->addWidget(m_importText);

        auto* row = new QHBoxLayout;
        auto* applyBtn = new QPushButton("Apply Import →");
        applyBtn->setProperty("role", "primary");
        connect(applyBtn, &QPushButton::clicked, this, &SetupTab::applyImport);
        auto* clrBtn = new QPushButton("Clear Text");
        connect(clrBtn, &QPushButton::clicked, m_importText, &QTextEdit::clear);
        row->addWidget(applyBtn); row->addWidget(clrBtn); row->addStretch();
        vl->addLayout(row);
        return w;
    }

    void applyImport() {
        auto* plan = DataManager::instance().planById(m_planId);
        if (!plan) { QMessageBox::warning(this, "No Plan", "Select a plan first."); return; }
        QString text = m_importText->toPlainText().trimmed();
        if (text.isEmpty()) return;

        QStringList lines = text.split('\n');
        if (!plan->quests.isEmpty())
            ; // keep existing quests, overwrite
        QMap<int,Quest*> qmap;
        for (auto& q : plan->quests) qmap[q.day] = &q;

        int imported = 0;
        for (int i = 0; i < lines.size(); i++) {
            int day = i + 1;
            if (day > plan->duration) break;
            QString line = lines[i].trimmed();
            if (line.isEmpty()) continue;

            bool isMs = line.startsWith("★") || line.startsWith("*");
            QString raw = line;
            raw.remove(QRegularExpression("^[★\\*]\\s*"));
            int xp = isMs ? 500 : 100;
            int pipeIdx = raw.lastIndexOf('|');
            if (pipeIdx >= 0) {
                bool ok; int v = raw.mid(pipeIdx+1).trimmed().toInt(&ok);
                if (ok && v > 0) { xp = std::clamp(v, 1, 9999); raw = raw.left(pipeIdx).trimmed(); }
            }
            if (raw.isEmpty()) continue;

            if (qmap.contains(day)) {
                qmap[day]->title       = raw;
                qmap[day]->isMilestone = isMs;
                qmap[day]->xp          = xp;
                if (isMs) qmap[day]->milestoneTitle = raw;
            } else {
                Quest q; q.day = day; q.title = raw;
                q.xp = xp; q.isMilestone = isMs;
                if (isMs) q.milestoneTitle = raw;
                plan->quests.append(q);
            }
            imported++;
        }
        std::sort(plan->quests.begin(), plan->quests.end(),
                  [](const Quest& a, const Quest& b){ return a.day < b.day; });
        DataManager::instance().save();
        populateTable();
        QMessageBox::information(this, "Done", QString("Imported %1 topics.").arg(imported));
    }

    // ── Plan management ──────────────────────────────
    void newPlanDialog() {
        QDialog dlg(this);
        dlg.setWindowTitle("Create New Quest Line");
        dlg.setMinimumWidth(400);
        auto* vl = new QVBoxLayout(&dlg);
        vl->setSpacing(10); vl->setContentsMargins(20,20,20,20);

        vl->addWidget([]{ auto* l = new QLabel("Quest Line Name *"); l->setStyleSheet("border:none;"); return l; }());
        auto* ne = new QLineEdit; ne->setPlaceholderText("e.g. C++ Mastery — 120 Days");
        vl->addWidget(ne);
        vl->addWidget([]{ auto* l = new QLabel("Description"); l->setStyleSheet("border:none;"); return l; }());
        auto* de = new QLineEdit; de->setPlaceholderText("Optional");
        vl->addWidget(de);

        auto* dateRow = new QHBoxLayout;
        auto* sc = new QVBoxLayout; sc->addWidget([]{ auto* l = new QLabel("Start Date"); l->setStyleSheet("border:none;"); return l; }());
        auto* sde = new QDateEdit(QDate::currentDate()); sde->setCalendarPopup(true);
        sc->addWidget(sde);
        auto* dc = new QVBoxLayout; dc->addWidget([]{ auto* l = new QLabel("Duration (days)"); l->setStyleSheet("border:none;"); return l; }());
        auto* ds = new QSpinBox; ds->setRange(1,3650); ds->setValue(120);
        dc->addWidget(ds);
        dateRow->addLayout(sc); dateRow->addLayout(dc);
        vl->addLayout(dateRow);

        auto* btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        vl->addWidget(btns);

        if (dlg.exec() != QDialog::Accepted) return;
        QString name = ne->text().trimmed();
        if (name.isEmpty()) { QMessageBox::warning(this, "Error", "Name is required."); return; }

        Plan p;
        p.id          = QUuid::createUuid().toString(QUuid::WithoutBraces);
        p.name        = name;
        p.description = de->text().trimmed();
        p.startDate   = sde->date().toString("yyyy-MM-dd");
        p.duration    = ds->value();
        DataManager::instance().plans.append(p);
        DataManager::instance().activePlanIds.insert(p.id);
        DataManager::instance().save();
        emit planChanged();

        m_block = true;
        m_planCombo->addItem(p.name, p.id);
        m_planCombo->setCurrentIndex(m_planCombo->count() - 1);
        m_block = false;
        m_planId = p.id;
        loadCurrentPlan();
    }

    void deletePlan() {
        auto* plan = DataManager::instance().planById(m_planId);
        if (!plan) return;
        if (QMessageBox::question(this, "Delete", QString("Delete \"%1\"?\n\nAll progress will be erased.").arg(plan->name))
            != QMessageBox::Yes) return;
        auto& dm = DataManager::instance();
        dm.plans.erase(std::remove_if(dm.plans.begin(), dm.plans.end(),
            [&](const Plan& p){ return p.id == m_planId; }), dm.plans.end());
        dm.progress.remove(m_planId);
        dm.activePlanIds.remove(m_planId); // remove from active set if present
        dm.save();
        m_planId.clear();
        emit planChanged();
    }

    void onPlanSelected() {
        m_planId = m_planCombo->currentData().toString();
        
        DataManager::instance().save();
        loadCurrentPlan();
    }

    void loadCurrentPlan() {
        auto* plan = DataManager::instance().planById(m_planId);
        if (!plan) { m_table->setRowCount(0); return; }
        m_block = true;
        m_nameEdit->setText(plan->name);
        m_descEdit->setPlainText(plan->description);
        QDate d = QDate::fromString(plan->startDate, "yyyy-MM-dd");
        if (d.isValid()) m_startDate->setDate(d);
        m_durSpin->setValue(plan->duration);
        loadRankSpins(*plan);
        m_block = false;
        populateTable();
        updateCurvePreview();
    }

    void savePlanField(const QString& field, const QVariant& value) {
        if (m_block) return;
        auto* plan = DataManager::instance().planById(m_planId);
        if (!plan) return;

        if (field == "name") {
            plan->name = value.toString();
            m_planCombo->setItemText(m_planCombo->currentIndex(), plan->name);

        } else if (field == "description") {
            plan->description = value.toString();

        } else if (field == "start_date") {
            plan->startDate = value.toString();
            // Start date change shifts all day→calendar mappings — refresh table to update date column
            populateTable();

        } else if (field == "duration") {
            int newDur = value.toInt();
            auto& prog = DataManager::instance().progressFor(m_planId);

            // Check if completed/stalled days would be orphaned
            int maxCompleted = 0;
            for (int d : prog.completedDays)
                maxCompleted = std::max(maxCompleted, d);
            int maxStalled = 0;
            for (int d : prog.stalledDays)
                maxStalled = std::max(maxStalled, d);
            int maxUsed = std::max(maxCompleted, maxStalled);

            if (newDur < maxUsed) {
                auto reply = QMessageBox::question(this, "Duration Warning",
                    QString("You have completed or stalled days up to Day %1.\n\n"
                            "Reducing duration to %2 will remove those days from the plan.\n"
                            "Their XP will be subtracted and progress recalculated.\n\n"
                            "Continue?").arg(maxUsed).arg(newDur));
                if (reply != QMessageBox::Yes) {
                    m_block = true;
                    m_durSpin->setValue(plan->duration);
                    m_block = false;
                    return;
                }
                // ── Trim orphaned progress ────────────────────────────
                // Remove completed days beyond new duration
                QSet<int> toRemove;
                for (int d : prog.completedDays)
                    if (d > newDur) toRemove.insert(d);
                for (int d : toRemove) prog.completedDays.remove(d);

                // Remove stalled days beyond new duration
                QSet<int> stallRemove;
                for (int d : prog.stalledDays)
                    if (d > newDur) stallRemove.insert(d);
                for (int d : stallRemove) prog.stalledDays.remove(d);

                // Recalc total XP from remaining completed days
                int total = 0;
                for (int d : prog.completedDays) {
                    const Quest* q = plan->questForDay(d);
                    total += q ? q->xp : 100;
                }
                prog.totalXp = total;

                // Recalc streak from updated studyDates
                // (studyDates are calendar-based — we keep them intact;
                //  streak recalc from them is already correct)
                DataManager::recalcStreakPublic(prog);
            }

            // ── Compute XP totals BEFORE trimming (needed for threshold scaling) ──
            int xpOldTotal = Util::totalPlanXP(*plan); // plan->duration still = old value here

            // ── Trim quests beyond new duration ──────────────────────
            plan->quests.erase(
                std::remove_if(plan->quests.begin(), plan->quests.end(),
                    [&](const Quest& q){ return q.day > newDur; }),
                plan->quests.end());

            int oldDur = plan->duration;
            plan->duration = newDur;

            // XP total AFTER trimming
            int xpNew = Util::totalPlanXP(*plan);
            int xpOld = xpOldTotal > 0 ? xpOldTotal : xpNew;

            RankThresholds proposed = Util::scaledThresholds(
                plan->rankThresholds, xpOld, xpNew);

            // Only prompt if the proposed value differs meaningfully (>5% on S rank)
            bool differs = plan->rankThresholds.S > 0 &&
                std::abs(proposed.S - plan->rankThresholds.S) > plan->rankThresholds.S * 0.05;

            if (differs) {
                QString reason = plan->thresholdsManuallySet
                    ? "You've manually customised the rank thresholds."
                    : "Your rank thresholds were set automatically.";

                QString msg = QString(
                    "Duration changed from %1 to %2 days.\n"
                    "Total earnable XP: %3 → %4\n\n"
                    "%5\n\n"
                    "Would you like to rescale the rank thresholds?\n\n"
                    "  Current S-Rank threshold:  %6 XP\n"
                    "  Proposed S-Rank threshold: %7 XP  (= 100% completion)\n\n"
                    "'Yes' scales all ranks so completing the full plan earns S-Rank.\n"
                    "'No' keeps your existing thresholds — S-Rank may be unreachable.")
                    .arg(oldDur).arg(newDur)
                    .arg(xpOld).arg(xpNew)
                    .arg(reason)
                    .arg(plan->rankThresholds.S)
                    .arg(proposed.S);

                auto reply = QMessageBox::question(this,
                    "Adjust Rank Thresholds?", msg,
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::Yes);

                if (reply == QMessageBox::Yes) {
                    plan->rankThresholds = proposed;
                    loadRankSpins(*plan); // refresh spinboxes
                }
            }

            // When increasing duration, nothing to clean — new days are blank by default
            populateTable();

            // Propagate to whole app immediately
            DataManager::instance().save();
            emit planChanged();
            return; // save() already called above
        }

        DataManager::instance().save();
    }

    // Called from onXpChanged when a quest's XP is edited.
    // Recalculates totalXp if the affected day was already completed.
    void recalcXpAfterEdit(int day) {
        auto* plan = DataManager::instance().planById(m_planId);
        if (!plan) return;
        auto& prog = DataManager::instance().progressFor(m_planId);
        if (!prog.completedDays.contains(day)) return;

        // Full recalculation: sum XP for all completed days
        int total = 0;
        for (int d : prog.completedDays) {
            const Quest* q = plan->questForDay(d);
            total += q ? q->xp : 100;
        }
        prog.totalXp = total;
        DataManager::instance().save();
    }
};

// ============================================================
//  MANAGE TAB
// ============================================================
class ManageTab : public QWidget {
    Q_OBJECT
public:
    explicit ManageTab(QWidget* parent = nullptr) : QWidget(parent) {
        auto* root  = new QVBoxLayout(this);
        root->setContentsMargins(0,0,0,0);

        auto* scrollContent = new QWidget;
        scrollContent->setStyleSheet(QString("background: %1; border: none;").arg(Cl::cream));
        m_vl = new QVBoxLayout(scrollContent);
        m_vl->setContentsMargins(28,24,28,24); m_vl->setSpacing(14);

        auto* scroll = new QScrollArea;
        scroll->setWidgetResizable(true); scroll->setWidget(scrollContent);
        root->addWidget(scroll);

        // Plans group
        m_plansGrp = new QGroupBox("ALL QUEST LINES");
        m_plansLayout = new QVBoxLayout(m_plansGrp);
        m_plansLayout->setSpacing(10);
        m_vl->addWidget(m_plansGrp);

        // Backup
        auto* bakGrp = new QGroupBox("DATA BACKUP");
        auto* bakVl  = new QVBoxLayout(bakGrp);
        auto* bakTip = new QLabel("Export your plans and progress to JSON. Import to restore. Export weekly.");
        bakTip->setWordWrap(true);
        bakTip->setStyleSheet(QString("color: %1; font-size: 12px; border: none;").arg(Cl::ink3));
        bakVl->addWidget(bakTip);
        auto* bakRow = new QHBoxLayout;
        auto* expBtn = new QPushButton("Export JSON");
        connect(expBtn, &QPushButton::clicked, this, &ManageTab::exportData);
        auto* impBtn = new QPushButton("Import JSON");
        connect(impBtn, &QPushButton::clicked, this, &ManageTab::importData);
        bakRow->addWidget(expBtn); bakRow->addWidget(impBtn); bakRow->addStretch();
        bakVl->addLayout(bakRow);
        m_vl->addWidget(bakGrp);

        // About
        auto* abtGrp = new QGroupBox("ABOUT");
        auto* abtVl  = new QVBoxLayout(abtGrp);
        // dataPath() uses QStandardPaths — always points to the current user's own directory
        QString dataNote = QString(
            "<b>reSTEM</b> — Quest-based Learning Planner  v1.0<br>"
            "C++17 / Qt6  ·  Linux + macOS + Windows<br><br>"
            "Your data is stored locally on <b>this computer</b> only:<br>"
            "<code>%1</code><br><br>"
            "<span style='color:#8D9096;font-size:11px;'>"
            "This path is specific to your user account. Each person who runs this app "
            "on their own computer sees their own path and their own data.<br>"
            "Developed by - Mahmud Hassan Rimon<br>"
            "<b>CSE</b> @ University of Chittagong<br>"
            "contact:mahmudhassanrimoncu@gmail.com"
            "</span>").arg(DataManager::dataPath());
        auto* abtTxt = new QLabel(dataNote);
        abtTxt->setStyleSheet(QString("color: %1; font-size: 12px; border: none;").arg(Cl::ink3));
        abtTxt->setTextFormat(Qt::RichText);
        abtTxt->setWordWrap(true);
        abtVl->addWidget(abtTxt);
        m_vl->addWidget(abtGrp);
        m_vl->addStretch();
    }

    void refresh() {
        // Clear old cards
        QLayoutItem* item;
        while ((item = m_plansLayout->takeAt(0)) != nullptr) {
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }

        auto& dm = DataManager::instance();

        // Separate active vs archived
        QVector<const Plan*> active, archived;
        for (const auto& p : dm.plans)
            (p.archived ? archived : active).append(&p);

        if (active.isEmpty() && archived.isEmpty()) {
            auto* e = new QLabel("No quest lines yet. Go to Setup tab to create one.");
            e->setStyleSheet(QString("color: %1; border: none;").arg(Cl::ink4));
            m_plansLayout->addWidget(e);
            return;
        }

        // ── Active plans ──────────────────────────────────────────
        if (!active.isEmpty()) {
            auto* hdr = makeMgrHeader("ACTIVE PLANS", Cl::ink3);
            m_plansLayout->addWidget(hdr);
            for (const auto* plan : active)
                m_plansLayout->addWidget(buildPlanCard(*plan, false));
            m_plansLayout->addSpacing(16);
        }

        // ── Archived plans ────────────────────────────────────────
        if (!archived.isEmpty()) {
            auto* hdr = makeMgrHeader("ARCHIVED", Cl::ink4);
            m_plansLayout->addWidget(hdr);
            for (const auto* plan : archived)
                m_plansLayout->addWidget(buildPlanCard(*plan, true));
        }
    }

signals:
    void planChanged();
    void editPlanRequested(const QString& planId);

private:
    QVBoxLayout* m_vl          = nullptr;
    QGroupBox*   m_plansGrp    = nullptr;
    QVBoxLayout* m_plansLayout = nullptr;

    // ── Build a single plan card ──────────────────────────────────
    QFrame* buildPlanCard(const Plan& plan, bool isArchived) {
        auto& dm   = DataManager::instance();
        auto& prog = dm.progressFor(plan.id);
        int   pct  = Util::completionPct(plan, prog);
        auto  rank = Util::currentRank(prog.totalXp, plan);
        bool  isActive = dm.isPlanActive(plan.id);
        bool  isDone   = plan.isCompleteWithProgress(prog.completedDays);

        auto* card = new QFrame;
        QString bgColor  = isArchived ? Cl::cardBg2
                         : isActive   ? Cl::greenBg
                         : isDone     ? Cl::tealBg
                         : Cl::white_;
        QString bdrColor = isArchived ? Cl::border
                         : isActive   ? Cl::tealBdr
                         : isDone     ? Cl::tealBdr
                         : Cl::border;
        card->setStyleSheet(QString(
            "QFrame { background: %1; border: 1px solid %2; border-radius: 12px; }")
            .arg(bgColor).arg(bdrColor));
        auto* cl = new QVBoxLayout(card);
        cl->setContentsMargins(18,14,18,14); cl->setSpacing(8);

        // Top row: name + badges
        auto* tr = new QHBoxLayout;
        auto* nameLbl = new QLabel(plan.name);
        nameLbl->setStyleSheet(QString(
            "color: %1; font-size: 14px; font-weight: 700;"
            " border: none; background: transparent;")
            .arg(isArchived ? Cl::ink4 : Cl::ink));
        auto* rankBdg = new QLabel(QString("  %1-Rank  ").arg(rank.rank));
        rankBdg->setStyleSheet(QString(
            "color: %1; font-weight: 700; font-size: 11px;"
            " border: 2px solid %1; border-radius: 5px; padding: 2px 6px;"
            " background: %2;").arg(rank.color).arg(Cl::white_));

        if (isActive) {
            auto* t = new QLabel("ACTIVE");
            t->setStyleSheet(QString(
                "color: %1; font-size: 9px; font-weight: 700; letter-spacing: 2px;"
                " border: 1px solid %2; border-radius: 4px; padding: 2px 8px;"
                " background: %3;").arg(Cl::teal2).arg(Cl::tealBdr).arg(Cl::tealBg));
            tr->addWidget(t);
        }
        if (isDone && !isArchived) {
            auto* t = new QLabel("COMPLETE");
            t->setStyleSheet(QString(
                "color: %1; font-size: 9px; font-weight: 700; letter-spacing: 2px;"
                " border: 1px solid %2; border-radius: 4px; padding: 2px 8px;"
                " background: %3;").arg(Cl::teal2).arg(Cl::tealBdr).arg(Cl::tealBg));
            tr->addWidget(t);
        }
        if (isArchived) {
            auto* t = new QLabel("ARCHIVED");
            t->setStyleSheet(QString(
                "color: %1; font-size: 9px; font-weight: 700; letter-spacing: 2px;"
                " border: 1px solid %1; border-radius: 4px; padding: 2px 8px;"
                " background: %2;").arg(Cl::ink4).arg(Cl::cardBg2));
            tr->addWidget(t);
        }
        tr->addWidget(nameLbl); tr->addStretch(); tr->addWidget(rankBdg);
        cl->addLayout(tr);

        // Meta
        auto* meta = new QLabel(QString("%1 days  ·  starts %2  ·  %3 XP earned")
            .arg(plan.duration).arg(plan.startDate).arg(prog.totalXp));
        meta->setStyleSheet(QString(
            "color: %1; font-size: 11px; border: none; background: transparent;")
            .arg(Cl::ink4));
        cl->addWidget(meta);

        // Progress bar
        auto* prow = new QHBoxLayout;
        auto* pbar = new QProgressBar;
        pbar->setRange(0,100); pbar->setValue(pct);
        pbar->setFixedHeight(6); pbar->setTextVisible(false);
        pbar->setStyleSheet(QString(
            "QProgressBar { background: %1; border: none; border-radius: 3px; }"
            "QProgressBar::chunk { background: %2; border-radius: 3px; }")
            .arg(Cl::cardBg3).arg(Cl::green));
        auto* pctLbl = new QLabel(QString("%1%").arg(pct));
        pctLbl->setStyleSheet(QString(
            "color: %1; font-size: 11px; min-width: 36px;"
            " border: none; background: transparent;").arg(Cl::ink4));
        prow->addWidget(pbar); prow->addWidget(pctLbl);
        cl->addLayout(prow);

        // Action buttons
        auto* arow = new QHBoxLayout;
        if (!isArchived) {
            // Toggle active/inactive — always visible so any plan can be toggled
            auto* toggleBtn = new QPushButton(
                isActive ? svgIcon("check", 13, QColor(Cl::green)) : QIcon(),
                isActive ? "  Active" : "Set Active");
            toggleBtn->setFixedHeight(30);
            if (isActive) {
                toggleBtn->setStyleSheet(QString(
                    "QPushButton { background: %1; color: %2; border: 1px solid %3;"
                    " border-radius: 7px; font-size: 11px; font-weight: 600; }"
                    "QPushButton:hover { background: %4; border-color: %5; color: %5; }")
                    .arg(Cl::tealBg).arg(Cl::teal2).arg(Cl::tealBdr)
                    .arg(Cl::redBg).arg(Cl::red));
                toggleBtn->setToolTip("Click to deactivate this plan");
            }
            connect(toggleBtn, &QPushButton::clicked, this,
                [this, id = plan.id, isActive]{ toggleActive(id, isActive); });
            arow->addWidget(toggleBtn);
            auto* editBtn = new QPushButton("Edit Plan"); editBtn->setFixedHeight(30);
            connect(editBtn, &QPushButton::clicked, this,
                [this, id = plan.id]{ editPlan(id); });
            arow->addWidget(editBtn);

            auto* archBtn = new QPushButton(svgIcon("archive", 14), " Archive"); archBtn->setFixedHeight(30);
            connect(archBtn, &QPushButton::clicked, this,
                [this, id = plan.id]{ archivePlan(id); });
            arow->addWidget(archBtn);
        } else {
            auto* unarchBtn = new QPushButton("↩  Unarchive"); unarchBtn->setFixedHeight(30);
            connect(unarchBtn, &QPushButton::clicked, this,
                [this, id = plan.id]{ unarchivePlan(id); });
            arow->addWidget(unarchBtn);
        }

        auto* delBtn = new QPushButton("Delete");
        delBtn->setProperty("role","danger"); delBtn->setFixedHeight(30);
        connect(delBtn, &QPushButton::clicked, this,
            [this, id = plan.id]{ deletePlan(id); });
        arow->addWidget(delBtn); arow->addStretch();
        cl->addLayout(arow);

        return card;
    }

    static QWidget* makeMgrHeader(const QString& text, const QString& color) {
        auto* w = new QWidget; w->setFixedHeight(26);
        w->setStyleSheet("background: transparent; border: none;");
        auto* hl = new QHBoxLayout(w);
        hl->setContentsMargins(0, 0, 0, 0); hl->setSpacing(8);
        auto* lbl = new QLabel(text);
        lbl->setStyleSheet(QString(
            "color: %1; font-size: 10px; font-weight: 700; letter-spacing: 3px;"
            " border: none; background: transparent;").arg(color));
        auto* line = new QFrame; line->setFrameShape(QFrame::HLine);
        line->setStyleSheet(QString(
            "background: %1; max-height: 1px; border: none;").arg(Cl::border));
        line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        hl->addWidget(lbl); hl->addWidget(line, 1);
        return w;
    }

    void toggleActive(const QString& id, bool wasActive) {
        auto& dm = DataManager::instance();
        if (wasActive) {
            dm.activePlanIds.remove(id);
        } else {
            dm.activePlanIds.insert(id);
        }
        dm.save();
        emit planChanged();
    }

    void editPlan(const QString& id) {
        emit editPlanRequested(id);
    }

    void archivePlan(const QString& id) {
        auto* p = DataManager::instance().planById(id);
        if (!p) return;
        p->archived = true;
        // Remove from active set if archived
        DataManager::instance().activePlanIds.remove(id);
        DataManager::instance().save();
        emit planChanged();
    }

    void unarchivePlan(const QString& id) {
        auto* p = DataManager::instance().planById(id);
        if (!p) return;
        p->archived = false;
        DataManager::instance().save();
        emit planChanged();
    }

    void deletePlan(const QString& id) {
        auto* p = DataManager::instance().planById(id);
        if (!p) return;
        if (QMessageBox::question(this, "Delete Plan",
            QString("Permanently delete \"%1\"?\n\nAll progress will be erased."
                    "\n\nTip: Use Archive to keep the data without seeing it on Dashboard.")
            .arg(p->name)) != QMessageBox::Yes) return;
        auto& dm = DataManager::instance();
        dm.plans.erase(std::remove_if(dm.plans.begin(), dm.plans.end(),
            [&](const Plan& pl){ return pl.id == id; }), dm.plans.end());
        dm.progress.remove(id);
        dm.activePlanIds.remove(id);
        dm.save();
        emit planChanged();
    }

    void exportData() {
        QString path = QFileDialog::getSaveFileName(this, "Export Data",
            QDir::homePath() + "/the-system-backup.json", "JSON (*.json)");
        if (path.isEmpty()) return;
        QFile f(path);
        if (!f.open(QIODevice::WriteOnly)) { QMessageBox::critical(this, "Error", "Cannot write file."); return; }
        auto& dm = DataManager::instance();
        QJsonObject root;
        QJsonArray pa;
        for (const auto& p : dm.plans) pa.append(p.toJson());
        root["plans"] = pa;
        QJsonObject prog;
        for (auto it = dm.progress.begin(); it != dm.progress.end(); ++it)
            prog[it.key()] = it.value().toJson();
        root["progress"]       = prog;
        QJsonArray activeArr;
        for (const auto& id : dm.activePlanIds) activeArr.append(id);
        root["active_plan_ids"] = activeArr;
        root["active_plan_id"]  = dm.primaryActivePlanId(); // legacy compat
        root["version"]        = 1;
        f.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
        QMessageBox::information(this, "Exported", "Saved to " + path);
    }

    void importData() {
        QString path = QFileDialog::getOpenFileName(this, "Import Data", QDir::homePath(), "JSON (*.json)");
        if (path.isEmpty()) return;
        if (QMessageBox::question(this, "Import", "This will replace ALL current data. Continue?")
            != QMessageBox::Yes) return;
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly)) { QMessageBox::critical(this, "Error", "Cannot read file."); return; }
        auto doc = QJsonDocument::fromJson(f.readAll());
        if (doc.isNull()) { QMessageBox::critical(this, "Error", "Invalid JSON file."); return; }
        auto& dm  = DataManager::instance();
        auto root = doc.object();
        dm.plans.clear();
        for (const auto& v : root["plans"].toArray()) dm.plans.append(Plan::fromJson(v.toObject()));
        dm.progress.clear();
        auto prog = root["progress"].toObject();
        for (auto it = prog.begin(); it != prog.end(); ++it)
            dm.progress[it.key()] = Progress::fromJson(it.value().toObject());
        dm.activePlanIds.clear();
        if (root.contains("active_plan_ids")) {
            for (const auto& v : root["active_plan_ids"].toArray())
                dm.activePlanIds.insert(v.toString());
        } else if (root.contains("active_plan_id")) {
            QString id = root["active_plan_id"].toString();
            if (!id.isEmpty()) dm.activePlanIds.insert(id);
        }
        dm.save();
        emit planChanged();
        QMessageBox::information(this, "Imported", "Data restored successfully.");
    }
};

// ============================================================
//  CUSTOM PAINTED WIDGETS FOR STATS
// ============================================================

// ── Bar chart (horizontal or vertical, generic) ──────────────
struct BarData { QString label; double value; QString color; };

class BarChartWidget : public QWidget {
public:
    enum Orientation { Vertical, Horizontal };

    explicit BarChartWidget(Orientation ori = Vertical, QWidget* parent = nullptr)
        : QWidget(parent), m_ori(ori)
    {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    void setData(const QVector<BarData>& data, const QString& yLabel = "") {
        m_data   = data;
        m_yLabel = yLabel;
        update();
    }

    void setTitle(const QString& t) { m_title = t; update(); }

protected:
    void paintEvent(QPaintEvent*) override {
        if (m_data.isEmpty()) return;
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setRenderHint(QPainter::TextAntialiasing);

        const int W = width(), H = height();
        const int pad_l = m_ori == Horizontal ? 164 : 42;
        const int pad_r = m_ori == Horizontal ? 46 : 18;
        const int pad_t = m_title.isEmpty() ? 12 : 30;
        const int pad_b = m_ori == Horizontal ? 12 : 36;
        const int cw = W - pad_l - pad_r;
        const int ch = H - pad_t - pad_b;

        QFont labelFont; labelFont.setPointSize(9);
        QFont titleFont; titleFont.setPointSize(10); titleFont.setWeight(QFont::DemiBold);
        p.setFont(labelFont);

        double maxVal = 0;
        for (const auto& d : m_data) maxVal = std::max(maxVal, d.value);
        if (maxVal == 0) maxVal = 1;

        // Title
        if (!m_title.isEmpty()) {
            p.setFont(titleFont);
            p.setPen(QColor(Cl::ink));
            p.drawText(QRect(pad_l, 4, cw, 22), Qt::AlignLeft | Qt::AlignVCenter, m_title);
            p.setFont(labelFont);
        }

        // Grid lines
        p.setPen(QPen(QColor(Cl::border), 0.7));
        const int gridLines = 4;
        if (m_ori == Vertical) {
            for (int i = 0; i <= gridLines; i++) {
                int y = pad_t + ch - int(ch * i / gridLines);
                p.drawLine(pad_l, y, pad_l + cw, y);
                double v = maxVal * i / gridLines;
                p.setPen(QColor(Cl::ink4));
                p.drawText(QRect(0, y - 8, pad_l - 4, 16), Qt::AlignRight | Qt::AlignVCenter,
                           QString::number(int(v)));
                p.setPen(QPen(QColor(Cl::border), 0.7));
            }
        }

        // Bars
        int n = m_data.size();
        if (m_ori == Vertical) {
            double barW = double(cw) / n;
            double innerW = barW * 0.62;
            for (int i = 0; i < n; i++) {
                double frac = m_data[i].value / maxVal;
                int bh = int(ch * frac);
                int bx = pad_l + int(barW * i + (barW - innerW) / 2);
                int by = pad_t + ch - bh;

                // Bar with rounded top
                QPainterPath path;
                int r = std::min(4, bh / 2);
                path.addRoundedRect(bx, by, int(innerW), bh, r, r);
                p.fillPath(path, QColor(m_data[i].color.isEmpty() ? Cl::blue : m_data[i].color));

                // Value above bar
                if (m_data[i].value > 0) {
                    p.setPen(QColor(Cl::ink3));
                    p.drawText(QRect(bx - 4, by - 18, int(innerW) + 8, 16),
                               Qt::AlignCenter, QString::number(int(m_data[i].value)));
                }

                // X-axis label
                p.setPen(QColor(Cl::ink4));
                QRect lr(bx - int((barW - innerW) / 2), pad_t + ch + 6,
                         int(barW), pad_b - 8);
                p.drawText(lr, Qt::AlignCenter | Qt::TextWordWrap, m_data[i].label);
            }
        } else {
            // Horizontal bars
            double barH = double(ch) / n;
            double innerH = std::max(10.0, barH * 0.60);
            for (int i = 0; i < n; i++) {
                double frac = m_data[i].value / maxVal;
                int bw  = int(cw * frac);
                int by  = pad_t + int(barH * i + (barH - innerH) / 2);

                // Label on left — uses the full pad_l minus a small gap
                p.setPen(QColor(Cl::ink3));
                p.drawText(QRect(4, by, pad_l - 12, int(innerH)),
                           Qt::AlignRight | Qt::AlignVCenter,
                           m_data[i].label);

                // Background track (shows even when value = 0)
                p.setBrush(QColor(Cl::border));
                p.setPen(Qt::NoPen);
                int trackR = std::min(3, int(innerH) / 2);
                p.drawRoundedRect(pad_l, by, cw, int(innerH), trackR, trackR);

                // Bar (minimum 3px wide so it's always visible when value > 0)
                if (bw > 0) {
                    int drawW = std::max(3, bw);
                    QPainterPath path;
                    int r = std::min(3, int(innerH) / 2);
                    path.addRoundedRect(pad_l, by, drawW, int(innerH), r, r);
                    p.fillPath(path, QColor(m_data[i].color.isEmpty() ? Cl::blue : m_data[i].color));
                }

                // Value at end of bar (or at start if 0)
                p.setPen(QColor(Cl::ink3));
                QString valStr = QString::number(int(m_data[i].value)) + "%";
                int valX = pad_l + std::max(bw + 6, 6);
                p.drawText(QRect(valX, by, pad_r - 4, int(innerH)),
                           Qt::AlignLeft | Qt::AlignVCenter, valStr);
            }
        }
        p.end();
    }

private:
    QVector<BarData> m_data;
    Orientation      m_ori;
    QString          m_title, m_yLabel;
};

// ── Ring / donut chart ────────────────────────────────────────
struct RingSegment { QString label; double value; QString color; };

class RingChartWidget : public QWidget {
public:
    explicit RingChartWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setMinimumSize(160, 160);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    void setData(const QVector<RingSegment>& segs, const QString& centre = "") {
        m_segs = segs; m_centre = centre; update();
    }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        int sz  = std::min(width(), height()) - 8;
        int ox  = (width()  - sz) / 2;
        int oy  = (height() - sz) / 2;
        QRectF outerRect(ox, oy, sz, sz);
        double thick = sz * 0.18;
        QRectF innerRect = outerRect.adjusted(thick, thick, -thick, -thick);

        double total = 0;
        for (const auto& s : m_segs) total += s.value;

        if (total == 0) {
            // Draw a muted placeholder ring + "No data" text
            p.setPen(QPen(QColor(Cl::border2), 2.0));
            p.setBrush(Qt::NoBrush);
            p.drawEllipse(outerRect.adjusted(1,1,-1,-1));
            p.setBrush(QColor(Cl::cardBg2));
            p.setPen(Qt::NoPen);
            p.drawEllipse(innerRect);
            QFont f; f.setPointSize(9); f.setWeight(QFont::Medium);
            p.setFont(f);
            p.setPen(QColor(Cl::ink4));
            p.drawText(innerRect.toRect(), Qt::AlignCenter, "No\ndata");
            p.end();
            return;
        }

        double angle = -90.0;
        for (const auto& s : m_segs) {
            double span = s.value / total * 360.0;
            QPainterPath path;
            path.moveTo(outerRect.center());
            path.arcTo(outerRect, angle, span);
            path.closeSubpath();
            QPainterPath hole;
            hole.addEllipse(innerRect);
            p.fillPath(path.subtracted(hole), QColor(s.color));
            angle += span;
        }

        // Hole background
        p.setBrush(QColor(Cl::white_));
        p.setPen(Qt::NoPen);
        p.drawEllipse(innerRect);

        // Centre text
        if (!m_centre.isEmpty()) {
            QFont f; f.setPointSize(11); f.setWeight(QFont::DemiBold);
            p.setFont(f);
            p.setPen(QColor(Cl::ink));
            p.drawText(innerRect.toRect(), Qt::AlignCenter, m_centre);
        }
        p.end();
    }
private:
    QVector<RingSegment> m_segs;
    QString m_centre;
};

// ── Streak calendar (last N weeks, GitHub-style) ──────────────
class WeeklyCalendarWidget : public QWidget {
    static constexpr int CELL = 13, GAP = 3, WEEKS = 16;
public:
    explicit WeeklyCalendarWidget(QWidget* parent = nullptr) : QWidget(parent) {
        // Height is fixed (7 rows), width expands to fill available space
        int h = 7 * (CELL + GAP) + GAP + 22;
        setMinimumHeight(h);
        setMaximumHeight(h);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    void setData(const QSet<int>& completedDays, const QSet<int>& stalledDays,
                 const QString& startDate)
    {
        m_completed = completedDays;
        m_stalled   = stalledDays;
        m_startDate = startDate;
        update();
    }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        QFont f; f.setPointSize(8);
        p.setFont(f);

        QDate planStart = QDate::fromString(m_startDate, "yyyy-MM-dd");
        QDate today     = QDate::currentDate();

        // Compute how many weeks fit in the current widget width
        int labelW  = 22;
        int visWeeks = std::min(WEEKS, std::max(4, (width() - labelW - GAP) / (CELL + GAP)));
        // Use visWeeks instead of WEEKS for the grid
        QDate rollingStart = today.addDays(-(visWeeks * 7 - 1));
        while (rollingStart.dayOfWeek() != 1) rollingStart = rollingStart.addDays(-1);

        QDate gridStart;
        if (planStart.isValid() && planStart > rollingStart) {
            gridStart = planStart;
            while (gridStart.dayOfWeek() != 1) gridStart = gridStart.addDays(-1);
        } else {
            gridStart = rollingStart;
        }

        // Day-of-week labels
        QStringList dows = {"M","","W","","F","",""};
        for (int r = 0; r < 7; r++) {
            if (!dows[r].isEmpty()) {
                p.setPen(QColor(Cl::ink4));
                p.drawText(QRect(0, 20 + r*(CELL+GAP), 16, CELL),
                           Qt::AlignCenter, dows[r]);
            }
        }

        // Month labels along the top — only print when month changes
        QString lastMonth;
        for (int week = 0; week < visWeeks; week++) {
            QDate weekStart = gridStart.addDays(week * 7);
            QString monthAbbr = weekStart.toString("MMM");
            if (monthAbbr != lastMonth) {
                int x = labelW + GAP + week * (CELL + GAP);
                p.setPen(QColor(Cl::ink3));
                QFont mf; mf.setPointSize(7); p.setFont(mf);
                p.drawText(QRect(x, 4, (CELL+GAP)*3, 14), Qt::AlignLeft | Qt::AlignVCenter, monthAbbr);
                QFont bf; bf.setPointSize(8); p.setFont(bf);
                lastMonth = monthAbbr;
            }
        }

        for (int week = 0; week < visWeeks; week++) {
            for (int dow = 0; dow < 7; dow++) {
                QDate cell = gridStart.addDays(week * 7 + dow);
                int x = labelW + GAP + week * (CELL + GAP);
                int y = 20 + dow * (CELL + GAP);

                // Convert cell date to plan day number
                int dayNum = planStart.isValid() ? planStart.daysTo(cell) + 1 : -1;
                bool done    = dayNum > 0 && m_completed.contains(dayNum);
                bool stalled = dayNum > 0 && m_stalled.contains(dayNum);
                bool future  = cell > today;
                bool inPlan  = dayNum >= 1;

                QColor fill;
                if      (!inPlan || future) fill = QColor(Cl::cardBg2);
                else if (done)              fill = QColor(Cl::green);
                else if (stalled)           fill = QColor(Cl::gold2);
                else                        fill = QColor(Cl::cardBg3); // pending in plan

                p.setBrush(fill);
                p.setPen(cell == today
                    ? QPen(QColor(Cl::blue), 1.5)
                    : QPen(QColor(Cl::border), 0.5));
                p.drawRoundedRect(x, y, CELL, CELL, 2, 2);
            }
        }
        p.end();
    }
private:
    QSet<int> m_completed, m_stalled;
    QString   m_startDate;
};

// ── Mini stat tile ────────────────────────────────────────────
class StatTile : public QFrame {
public:
    StatTile(const QString& icon, const QString& value,
             const QString& label, const QString& sub,
             const QString& accentColor, QWidget* parent = nullptr)
        : QFrame(parent)
    {
        setStyleSheet(QString("QFrame { background: %1; border: 1px solid %2; border-radius: 14px; }")
                      .arg(Cl::white_).arg(Cl::border));
        auto* vl = new QVBoxLayout(this);
        vl->setContentsMargins(16, 14, 16, 14); vl->setSpacing(2);

        // Use SVG icon if we have a mapping, otherwise fall back to emoji text
        QWidget* iconLbl = nullptr;
        if (kEmojiToSvg.contains(icon)) {
            iconLbl = makeIcon(kEmojiToSvg[icon], 22, QColor(accentColor));
        } else {
            auto* lbl = new QLabel(icon);
            lbl->setStyleSheet("font-size: 20px; border: none; background: transparent;");
            iconLbl = lbl;
        }

        m_valueLbl = new QLabel(value);
        m_valueLbl->setStyleSheet(QString("color: %1; font-size: 26px; font-weight: 700;"
            " border: none; background: transparent;").arg(accentColor));

        auto* labelLbl = new QLabel(label.toUpper());
        labelLbl->setStyleSheet(QString("color: %1; font-size: 9px; font-weight: 600;"
            " letter-spacing: 2px; border: none; background: transparent;").arg(Cl::ink4));

        m_subLbl = new QLabel(sub);
        m_subLbl->setStyleSheet(QString("color: %1; font-size: 10px; border: none;"
            " background: transparent;").arg(Cl::ink4));
        m_subLbl->setWordWrap(true);

        vl->addWidget(iconLbl);
        vl->addWidget(m_valueLbl);
        vl->addWidget(labelLbl);
        vl->addSpacing(2);
        vl->addWidget(m_subLbl);
    }

    void update(const QString& value, const QString& sub) {
        m_valueLbl->setText(value);
        m_subLbl->setText(sub);
    }

private:
    QLabel* m_valueLbl;
    QLabel* m_subLbl;
};

// ============================================================
//  STATISTICS TAB
// ============================================================
class StatsTab : public QWidget {
    Q_OBJECT
public:
    explicit StatsTab(QWidget* parent = nullptr) : QWidget(parent) {
        auto* root = new QVBoxLayout(this);
        root->setContentsMargins(0,0,0,0); root->setSpacing(0);

        // ── Toolbar: plan selector ────────────────────────────────
        auto* bar = new QWidget;
        bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        bar->setStyleSheet(QString("background: %1; border-bottom: 1px solid %2;")
                           .arg(Cl::white_).arg(Cl::border));
        auto* bl = new QHBoxLayout(bar);
        bl->setContentsMargins(24, 12, 24, 12);
        auto* barTitle = new QLabel("Statistics");
        barTitle->setStyleSheet(QString("font-size: 17px; font-weight: 700;"
            " letter-spacing: 2px; color: %1; border: none;").arg(Cl::navy));
        bl->addWidget(barTitle);
        bl->addSpacing(20);

        auto* planLbl = new QLabel("Plan:");
        planLbl->setStyleSheet(QString("color: %1; font-size: 11px; border: none;").arg(Cl::ink4));
        m_planCombo = new QComboBox;
        m_planCombo->setMinimumWidth(180);
        m_planCombo->setMaximumWidth(380);
        m_planCombo->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        connect(m_planCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this](int) { if (!m_block) buildContent(); });
        bl->addWidget(planLbl); bl->addWidget(m_planCombo); bl->addStretch();
        root->addWidget(bar);

        // ── Scrollable content area ───────────────────────────────
        m_contentWidget = new QWidget;
        m_contentWidget->setStyleSheet(
            QString("background: %1;").arg(Cl::cream));
        m_contentLayout = new QVBoxLayout(m_contentWidget);
        m_contentLayout->setContentsMargins(24, 20, 24, 24);
        m_contentLayout->setSpacing(16);

        auto* scroll = new QScrollArea;
        scroll->setWidgetResizable(true);
        scroll->setWidget(m_contentWidget);
        root->addWidget(scroll);
    }

    void refresh() {
        m_block = true;
        QString prev = m_planCombo->currentData().toString();
        m_planCombo->clear();

        // First entry is always the aggregate overview
        m_planCombo->addItem("📊  All Plans (Overview)", QVariant("__overall__"));

        for (const auto& p : DataManager::instance().plans)
            m_planCombo->addItem(p.name, p.id);  // include archived for stats

        // Restore previous selection, or default to overview
        int idx = 0;
        if (!prev.isEmpty() && prev != "__overall__")
            idx = std::max(0, m_planCombo->findData(prev));
        m_planCombo->setCurrentIndex(idx);

        m_block = false;
        buildContent();
    }

private:
    QComboBox*   m_planCombo     = nullptr;
    QWidget*     m_contentWidget = nullptr;
    QVBoxLayout* m_contentLayout = nullptr;
    bool         m_block         = false;

    // ── Helper: clear content ─────────────────────────────────────
    void clearContent() {
        QLayoutItem* item;
        while ((item = m_contentLayout->takeAt(0)) != nullptr) {
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }
    }

    // ── Compute all stats for selected plan ───────────────────────
    struct Stats {
        // Completion
        int   totalDays, doneDays, stalledDays_, remainingDays;
        int   completionPct;
        int   futureDays;

        // XP
        int   totalXp, xpFromCompleted, xpPossibleRemaining;
        float avgXpPerDay;
        int   highestXpDay;      QString highestXpTitle;
        int   lowestXpDay;       QString lowestXpTitle;
        int   milestoneXpTotal;
        int   milestonesCleared, milestonesTotal;

        // Streaks
        int   streakCurrent, streakBest;
        int   longestGap;       // longest gap between completions
        int   avgDaysBetween;

        // Pace
        float daysElapsed;
        float expectedPct;      // if on-track linearly
        float paceRatio;        // actual/expected  >1 = ahead
        int   projectedFinishDay;
        QDate projectedFinishDate;

        // Distribution: completions per day-of-week (0=Mon..6=Sun)
        QVector<int> dowCounts = QVector<int>(7, 0);

        // Weekly completion rate (last 8 weeks, 0=oldest)
        QVector<double> weeklyRate = QVector<double>(8, 0.0);

        // Per-phase XP breakdown (split by milestones)
        struct Phase { QString name; int xpEarned; int xpTotal; int days; int done; };
        QVector<Phase> phases;

        // Rank journey
        QString currentRankLabel, currentRankColor;
        int     xpToNextRank;
        QString nextRankLabel;
        int     rankPct;

        // Stall analysis
        int maxStallRun;         // longest consecutive stall run
    };

    Stats computeStats(const Plan& plan, const Progress& prog) {
        Stats s{};
        auto& dm   = DataManager::instance();
        int today  = Util::todayDayNum(plan);
        s.totalDays     = plan.duration;
        // Only count days within current plan duration (orphaned days ignored)
        s.doneDays      = Plan::validCompletedCount(prog.completedDays, plan.duration);
        s.stalledDays_  = [&]{ int c=0; for(int d:prog.stalledDays) if(d<=plan.duration) c++; return c; }();
        s.futureDays    = std::max(0, plan.duration - today);
        s.remainingDays = std::max(0, plan.duration - s.doneDays);
        s.completionPct = Util::completionPct(plan, prog);
        s.streakCurrent = prog.streakCurrent;
        s.streakBest    = prog.streakBest;

        // XP
        s.totalXp = prog.totalXp;
        s.xpFromCompleted = 0;
        s.xpPossibleRemaining = 0;
        s.milestoneXpTotal = 0;
        s.milestonesTotal  = 0;
        s.milestonesCleared= 0;
        s.highestXpDay = 0; s.lowestXpDay = 0;
        int highXp = -1, lowXp = INT_MAX;

        for (const auto& q : plan.quests) {
            if (q.isMilestone) {
                s.milestonesTotal++;
                if (prog.completedDays.contains(q.day)) {
                    s.milestonesCleared++;
                    s.milestoneXpTotal += q.xp;
                }
            }
            if (prog.completedDays.contains(q.day)) {
                s.xpFromCompleted += q.xp;
                if (q.xp > highXp) {
                    highXp = q.xp;
                    s.highestXpDay = q.day;
                    s.highestXpTitle = q.title;
                }
                if (q.xp < lowXp) {
                    lowXp = q.xp;
                    s.lowestXpDay = q.day;
                    s.lowestXpTitle = q.title;
                }
            } else if (!prog.stalledDays.contains(q.day)) {
                s.xpPossibleRemaining += q.xp;
            }
        }
        // Days without explicit quest entry default to 100 XP
        for (int d = 1; d <= plan.duration; d++) {
            if (!plan.questForDay(d)) {
                if (prog.completedDays.contains(d))  s.xpFromCompleted += 100;
                else if (!prog.stalledDays.contains(d)) s.xpPossibleRemaining += 100;
            }
        }

        s.avgXpPerDay = s.doneDays > 0 ? float(s.totalXp) / s.doneDays : 0;

        // Longest gap between completions
        QList<int> sortedDone = prog.completedDays.values();
        std::sort(sortedDone.begin(), sortedDone.end());
        s.longestGap = 0; s.avgDaysBetween = 0;
        if (sortedDone.size() >= 2) {
            int gapSum = 0;
            for (int i = 1; i < sortedDone.size(); i++) {
                int g = sortedDone[i] - sortedDone[i-1] - 1;
                s.longestGap = std::max(s.longestGap, g);
                gapSum += g;
            }
            int denom = int(sortedDone.size()) - 1;
            s.avgDaysBetween = denom > 0 ? gapSum / denom : 0;
        }

        // Max stall run
        QList<int> sortedStall = prog.stalledDays.values();
        std::sort(sortedStall.begin(), sortedStall.end());
        s.maxStallRun = 0;
        if (!sortedStall.isEmpty()) {
            int run = 1;
            for (int i = 1; i < sortedStall.size(); i++) {
                if (sortedStall[i] == sortedStall[i-1]+1) { run++; s.maxStallRun = std::max(s.maxStallRun, run); }
                else run = 1;
            }
            s.maxStallRun = std::max(s.maxStallRun, run);
        }

        // Pace — avoid misleading "Ahead by 0%" on day 1
        s.daysElapsed = std::max(0, today - 1);
        s.expectedPct = plan.duration > 0
            ? std::min(100.0f, float(s.daysElapsed) / plan.duration * 100.0f) : 0;
        if (s.daysElapsed == 0 || s.expectedPct < 0.01f) {
            // Plan just started — pace is meaningless, show neutral
            s.paceRatio = 1.0f;
        } else {
            s.paceRatio = float(s.completionPct) / s.expectedPct;
        }

        // Projected finish
        if (s.doneDays > 0 && s.daysElapsed > 0) {
            float daysPerCompletion = s.daysElapsed / s.doneDays;
            int remaining = plan.duration - s.doneDays;
            s.projectedFinishDay = today + int(remaining * daysPerCompletion);
            QDate startDate = QDate::fromString(plan.startDate, "yyyy-MM-dd");
            if (startDate.isValid())
                s.projectedFinishDate = startDate.addDays(s.projectedFinishDay - 1);
        } else {
            s.projectedFinishDay = plan.duration;
            QDate startDate = QDate::fromString(plan.startDate, "yyyy-MM-dd");
            s.projectedFinishDate = startDate.isValid()
                ? startDate.addDays(plan.duration) : QDate();
        }

        // Day-of-week distribution
        QDate planStart = QDate::fromString(plan.startDate, "yyyy-MM-dd");
        if (planStart.isValid()) {
            for (int d : prog.completedDays) {
                QDate dd = planStart.addDays(d - 1);
                int dow = dd.dayOfWeek() - 1;  // 0=Mon..6=Sun
                if (dow >= 0 && dow < 7) s.dowCounts[dow]++;
            }
        }

        // Weekly completion rate (last 8 weeks of plan days)
        if (planStart.isValid()) {
            QDate wEnd = QDate::currentDate();
            for (int w = 7; w >= 0; w--) {
                QDate ws = wEnd.addDays(-w * 7 - 6);
                QDate we = wEnd.addDays(-w * 7);
                int possible = 0, completed = 0;
                for (QDate d = ws; d <= we; d = d.addDays(1)) {
                    int dn = planStart.daysTo(d) + 1;
                    if (dn >= 1 && dn <= plan.duration) {
                        possible++;
                        if (prog.completedDays.contains(dn)) completed++;
                    }
                }
                int idx = 7 - w;
                if (idx >= 0 && idx < 8)
                    s.weeklyRate[idx] = possible > 0 ? double(completed) / possible * 100.0 : 0;
            }
        }

        // Phase breakdown: sections between milestones
        {
            QList<int> msdays;
            for (const auto& q : plan.quests)
                if (q.isMilestone) msdays.append(q.day);
            std::sort(msdays.begin(), msdays.end());
            msdays.prepend(0);
            if (msdays.last() < plan.duration) msdays.append(plan.duration);

            for (int i = 1; i < msdays.size(); i++) {
                int from = msdays[i-1] + 1, to = msdays[i];
                Stats::Phase ph;
                const Quest* msq = plan.questForDay(to);
                ph.name = msq && msq->isMilestone
                    ? (msq->milestoneTitle.isEmpty() ? msq->title : msq->milestoneTitle)
                    : QString("Days %1–%2").arg(from).arg(to);
                ph.days = to - from + 1;
                ph.done = 0; ph.xpEarned = 0; ph.xpTotal = 0;
                for (int d = from; d <= to; d++) {
                    const Quest* q = plan.questForDay(d);
                    int qxp = q ? q->xp : 100;
                    ph.xpTotal += qxp;
                    if (prog.completedDays.contains(d)) {
                        ph.done++;
                        ph.xpEarned += qxp;
                    }
                }
                s.phases.append(ph);
            }
        }

        // Rank info
        auto rank = Util::currentRank(prog.totalXp, plan);
        auto nxt  = Util::nextRank(prog.totalXp, plan);
        s.currentRankLabel = rank.rank + "-Rank · " + rank.label;
        s.currentRankColor = rank.color;
        s.rankPct = Util::xpProgress(prog.totalXp, plan).pct;
        if (nxt) {
            s.xpToNextRank   = nxt->xp - prog.totalXp;
            s.nextRankLabel  = nxt->rank + "-Rank · " + nxt->label;
        } else {
            s.xpToNextRank  = 0;
            s.nextRankLabel = "Max rank achieved";
        }

        return s;
    }

    // ── Overall across ALL plans ──────────────────────────────────
    void buildOverallContent() {
        auto& dm = DataManager::instance();
        if (dm.plans.isEmpty()) {
            auto* empty = new QLabel("No quest lines yet.\nCreate one in the Setup tab.");
            empty->setAlignment(Qt::AlignCenter);
            empty->setStyleSheet(QString("color: %1; font-size: 14px; border: none;").arg(Cl::ink4));
            m_contentLayout->addStretch();
            m_contentLayout->addWidget(empty);
            m_contentLayout->addStretch();
            return;
        }

        // ── Aggregate data collection ─────────────────────────────
        int totalPlans        = 0;
        int completedPlans    = 0;
        int activePlans       = 0;
        int archivedPlans     = 0;
        int totalDaysAll      = 0;
        int doneDaysAll       = 0;
        int stalledDaysAll    = 0;
        int totalXpAll        = 0;
        int milestonesAll     = 0;
        int milestonesCleared = 0;
        int bestStreakAll      = 0;
        int currentStreakAll   = 0;
        QVector<int> dowAll(7, 0);
        QVector<double> weeklyAll(8, 0.0);
        QVector<int> weeklyPossible(8, 0);
        QVector<int> weeklyDone(8, 0);

        // Per-plan summary rows
        struct PlanRow {
            QString name, rankLabel, rankColor;
            int doneDays, totalDays, totalXp, streak;
            int milestonesCleared, milestonesTotal;
            bool complete, archived;
            float completionPct;
            float satisfactionScore; // (done - stalled) / total * 100, capped 0-100
        };
        QVector<PlanRow> planRows;

        for (const auto& plan : dm.plans) {
            const Progress& prog = dm.progressFor(plan.id);
            int done    = Plan::validCompletedCount(prog.completedDays, plan.duration);
            int stalled = int(prog.stalledDays.size());
            int total   = plan.duration;
            bool complete = plan.isCompleteWithProgress(prog.completedDays);

            totalPlans++;
            if (plan.archived) archivedPlans++;
            else if (complete)  completedPlans++;
            else                activePlans++;

            totalDaysAll   += total;
            doneDaysAll    += done;
            stalledDaysAll += stalled;
            totalXpAll     += prog.totalXp;
            bestStreakAll   = std::max(bestStreakAll, prog.streakBest);
            currentStreakAll= std::max(currentStreakAll, prog.streakCurrent);

            for (const auto& q : plan.quests) {
                if (q.isMilestone) {
                    milestonesAll++;
                    if (prog.completedDays.contains(q.day)) milestonesCleared++;
                }
            }

            // Day-of-week distribution
            QDate planStart = QDate::fromString(plan.startDate, "yyyy-MM-dd");
            if (planStart.isValid()) {
                for (int d : prog.completedDays) {
                    int dow = planStart.addDays(d-1).dayOfWeek() - 1;
                    if (dow >= 0 && dow < 7) dowAll[dow]++;
                }

                // Weekly rate contribution
                QDate wEnd = QDate::currentDate();
                for (int w = 0; w < 8; w++) {
                    QDate ws = wEnd.addDays(-(7-w)*7 - 6);
                    QDate we = wEnd.addDays(-(7-w)*7);
                    for (QDate d = ws; d <= we; d = d.addDays(1)) {
                        int dn = planStart.daysTo(d) + 1;
                        if (dn >= 1 && dn <= plan.duration) {
                            weeklyPossible[w]++;
                            if (prog.completedDays.contains(dn)) weeklyDone[w]++;
                        }
                    }
                }
            }

            // Satisfaction score: reward completions, penalise stalls
            // 100 = all done perfectly, 0 = nothing done or all stalled
            float sat = total > 0
                ? std::clamp(float(done - stalled*0.5f) / total * 100.0f, 0.0f, 100.0f)
                : 0.0f;

            auto rank = Util::currentRank(prog.totalXp, plan);
            planRows.append({
                plan.name, rank.rank + " · " + rank.label, rank.color,
                done, total, prog.totalXp, prog.streakBest,
                [&]{ int c=0; for (auto& q:plan.quests) if(q.isMilestone&&prog.completedDays.contains(q.day)) c++; return c; }(),
                [&]{ int c=0; for (auto& q:plan.quests) if(q.isMilestone) c++; return c; }(),
                complete, plan.archived, total>0 ? float(done)/total*100.0f : 0.0f, sat
            });
        }

        for (int w = 0; w < 8; w++)
            weeklyAll[w] = weeklyPossible[w] > 0
                ? double(weeklyDone[w]) / weeklyPossible[w] * 100.0 : 0;

        float overallPct      = totalDaysAll > 0 ? float(doneDaysAll) / totalDaysAll * 100.0f : 0;
        float avgSatisfaction = 0;
        if (!planRows.isEmpty()) {
            float sum = 0; for (const auto& r : planRows) sum += r.satisfactionScore;
            avgSatisfaction = sum / planRows.size();
        }

        // ── Section 1: Overview headline tiles ───────────────────
        m_contentLayout->addWidget(makeSectionLabel("Overall Summary"));

        auto* tilesGrid = new QGridLayout;
        tilesGrid->setSpacing(12);
        for (int c = 0; c < 4; c++) tilesGrid->setColumnStretch(c, 1);

        // Satisfaction grade
        auto satGrade = [](float s) -> QPair<QString,QString> {
            if (s >= 90) return {"S", Cl::red};
            if (s >= 75) return {"A", Cl::gold2};
            if (s >= 60) return {"B", Cl::purple};
            if (s >= 45) return {"C", Cl::blue};
            if (s >= 30) return {"D", Cl::green};
            return {"E", Cl::ink4};
        };
        auto [satLetter, satColor] = satGrade(avgSatisfaction);

        struct T { QString icon,val,lbl,sub,color; };
        QVector<T> tiles = {
            {"📋", QString::number(totalPlans),
             "Total Plans",
             QString("%1 active  ·  %2 done  ·  %3 archived")
                .arg(activePlans).arg(completedPlans).arg(archivedPlans),
             Cl::navy},
            {"✅", QString("%1%").arg(int(overallPct)),
             "Overall Completion",
             QString("%1 / %2 days completed across all plans")
                .arg(doneDaysAll).arg(totalDaysAll),
             overallPct >= 80 ? Cl::green2 : overallPct >= 50 ? Cl::blue : Cl::ink3},
            {"⚡", QString::number(totalXpAll),
             "Total XP Earned",
             QString("Across %1 plan%2").arg(totalPlans).arg(totalPlans==1?"":"s"),
             Cl::purple2},
            {"🏆", QString("%1 / %2").arg(milestonesCleared).arg(milestonesAll),
             "Milestones",
             milestonesAll > 0
                ? QString("%1% checkpoints cleared")
                    .arg(int(float(milestonesCleared)/milestonesAll*100))
                : "No milestones set",
             Cl::gold2},
            {"🔥", QString::number(bestStreakAll),
             "Best Streak (any plan)",
             QString("Current best active: %1 days").arg(currentStreakAll),
             Cl::green},
            {"⏸",  QString::number(stalledDaysAll),
             "Total Stalled Days",
             stalledDaysAll > 0
                ? QString("%1% of all days stalled")
                    .arg(int(float(stalledDaysAll)/std::max(1,totalDaysAll)*100))
                : "None — clean run",
             Cl::gold2},
            {satLetter, QString("%1%").arg(int(avgSatisfaction)),
             "Avg Satisfaction",
             "Score = (done − stalls×0.5) / total",
             satColor},
            {"📅", QString::number(doneDaysAll),
             "Days Completed",
             QString("%1 remaining across all plans")
                .arg(totalDaysAll - doneDaysAll),
             Cl::green2},
        };
        for (int i = 0; i < tiles.size(); i++) {
            const auto& t = tiles[i];
            auto* tile = new StatTile(t.icon, t.val, t.lbl, t.sub, t.color);
            tilesGrid->addWidget(tile, i/4, i%4);
        }
        auto* tilesW = new QWidget;
        tilesW->setStyleSheet("background: transparent; border: none;");
        tilesW->setLayout(tilesGrid);
        m_contentLayout->addWidget(tilesW);

        // ── Section 2: Per-plan comparison table ──────────────────
        m_contentLayout->addWidget(makeSectionLabel("Plan-by-Plan Comparison"));
        auto* tblCard = makeCard();

        auto* tbl = new QTableWidget(int(planRows.size()), 7);
        tbl->setHorizontalHeaderLabels({
            "PLAN NAME", "RANK", "COMPLETION", "DAYS DONE", "TOTAL XP",
            "STREAK", "SATISFACTION"
        });
        tbl->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        tbl->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
        tbl->setColumnWidth(1, 130);
        for (int c = 2; c <= 6; c++) {
            tbl->horizontalHeader()->setSectionResizeMode(c, QHeaderView::ResizeToContents);
        }
        tbl->verticalHeader()->setVisible(false);
        tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tbl->setAlternatingRowColors(true);
        tbl->setStyleSheet(tbl->styleSheet() +
            QString("QTableWidget{alternate-background-color:%1;}").arg(Cl::cardBg2));

        // Sort by completion % descending
        QVector<int> order(planRows.size());
        std::iota(order.begin(), order.end(), 0);
        std::sort(order.begin(), order.end(), [&](int a, int b){
            return planRows[a].completionPct > planRows[b].completionPct;
        });

        for (int row = 0; row < int(order.size()); row++) {
            const auto& r = planRows[order[row]];
            auto cell = [&](const QString& txt, Qt::AlignmentFlag align = Qt::AlignLeft) {
                auto* it = new QTableWidgetItem(txt);
                it->setFlags(Qt::ItemIsEnabled);
                it->setTextAlignment(align | Qt::AlignVCenter);
                return it;
            };

            // Plan name + status tag
            QString nameStr = r.name;
            if (r.archived)  nameStr += "  [archived]";
            else if (r.complete) nameStr += "  ✓";
            auto* nameIt = cell(nameStr);
            if (r.complete)  nameIt->setForeground(QColor(Cl::green2));
            if (r.archived)  nameIt->setForeground(QColor(Cl::ink4));
            tbl->setItem(row, 0, nameIt);

            // Rank
            auto* rankIt = cell(r.rankLabel, Qt::AlignCenter);
            rankIt->setForeground(QColor(r.rankColor));
            tbl->setItem(row, 1, rankIt);

            // Completion %
            QString pctStr = QString("%1%").arg(int(r.completionPct));
            auto* pctIt = cell(pctStr, Qt::AlignCenter);
            if (r.completionPct >= 100) pctIt->setForeground(QColor(Cl::green2));
            else if (r.completionPct >= 50) pctIt->setForeground(QColor(Cl::blue));
            else pctIt->setForeground(QColor(Cl::ink3));
            tbl->setItem(row, 2, pctIt);

            tbl->setItem(row, 3, cell(
                QString("%1 / %2").arg(r.doneDays).arg(r.totalDays), Qt::AlignCenter));

            auto* xpIt = cell(QString::number(r.totalXp), Qt::AlignCenter);
            xpIt->setForeground(QColor(Cl::blue));
            tbl->setItem(row, 4, xpIt);

            tbl->setItem(row, 5, cell(
                r.streak > 0 ? QString("🔥 %1").arg(r.streak) : "—", Qt::AlignCenter));

            // Satisfaction with colour coding
            auto [sg, sc] = satGrade(r.satisfactionScore);
            auto* satIt = cell(
                QString("%1  %2%").arg(sg).arg(int(r.satisfactionScore)), Qt::AlignCenter);
            satIt->setForeground(QColor(sc));
            tbl->setItem(row, 6, satIt);

            tbl->setRowHeight(row, 34);
        }
        int tableH = std::min(int(planRows.size()) * 34 + 40, 320);
        tbl->setMinimumHeight(std::min(tableH, 200));
        tbl->setMaximumHeight(tableH);
        tblCard->layout()->addWidget(tbl);
        m_contentLayout->addWidget(tblCard);

        // ── Section 3: Combined weekly rate chart ─────────────────
        m_contentLayout->addWidget(makeSectionLabel("Combined Weekly Completion Rate"));
        auto* wkCard = makeCard();
        auto* wkChart = new BarChartWidget(BarChartWidget::Vertical);
        wkChart->setMinimumHeight(160);
        wkChart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QVector<BarData> wkData;
        QDate todayDate = QDate::currentDate();
        for (int i = 0; i < 8; i++) {
            QDate ws = todayDate.addDays(-(7-i)*7);
            QString col = weeklyAll[i] >= 70 ? Cl::teal
                        : weeklyAll[i] >= 40 ? Cl::blue : Cl::gold;
            wkData.append({ws.toString("MMM d"), weeklyAll[i], col});
        }
        wkChart->setData(wkData, "% days completed");
        wkCard->layout()->addWidget(wkChart);
        m_contentLayout->addWidget(wkCard);

        // ── Section 4: Day-of-week distribution (all plans) ───────
        m_contentLayout->addWidget(makeSectionLabel("Study Habit — Best Days (All Plans)"));
        auto* dowCard = makeCard();
        auto* dowRow  = new QHBoxLayout;

        auto* dowChart = new BarChartWidget(BarChartWidget::Vertical);
        dowChart->setMinimumHeight(140);
        dowChart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QStringList dowNames = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
        QVector<BarData> dowData;
        int maxDow = *std::max_element(dowAll.begin(), dowAll.end());
        for (int i = 0; i < 7; i++) {
            QString col = dowAll[i] == maxDow && maxDow > 0 ? Cl::teal : Cl::blue;
            dowData.append({dowNames[i], double(dowAll[i]), col});
        }
        dowChart->setData(dowData);

        int weekdayTotal = 0, weekendTotal = 0;
        for (int i = 0; i < 5; i++) weekdayTotal += dowAll[i];
        for (int i = 5; i < 7; i++) weekendTotal += dowAll[i];

        auto* ringChart = new RingChartWidget;
        ringChart->setMinimumSize(120, 120);
        ringChart->setMaximumSize(180, 180);
        ringChart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        ringChart->setData({
            {"Weekdays", double(weekdayTotal), Cl::blue},
            {"Weekend",  double(weekendTotal), Cl::teal},
        }, QString("%1\ndays").arg(doneDaysAll));

        auto* ringLeg = new QVBoxLayout;
        ringLeg->setSpacing(6); ringLeg->addStretch();
        for (const auto& seg : QVector<QPair<QString,QString>>{
                {Cl::purple, QString("Weekdays: %1").arg(weekdayTotal)},
                {Cl::green,  QString("Weekend:  %1").arg(weekendTotal)}}) {
            auto* lr = new QHBoxLayout;
            auto* dot = new QLabel; dot->setFixedSize(11, 11);
            dot->setStyleSheet(QString("background:%1;border-radius:2px;border:none;").arg(seg.first));
            auto* ll = new QLabel(seg.second);
            ll->setStyleSheet(QString("color:%1;font-size:11px;border:none;background:transparent;").arg(Cl::ink3));
            lr->addWidget(dot); lr->addWidget(ll); lr->addStretch();
            ringLeg->addLayout(lr);
        }
        ringLeg->addStretch();

        dowRow->addWidget(dowChart, 3);
        dowRow->addSpacing(16);
        dowRow->addWidget(ringChart);
        dowRow->addLayout(ringLeg);
        dowCard->layout()->addLayout(dowRow);
        m_contentLayout->addWidget(dowCard);

        // ── Section 5: Satisfaction breakdown ────────────────────
        m_contentLayout->addWidget(makeSectionLabel("Satisfaction Scores by Plan"));
        auto* satCard = makeCard();

        // Explain satisfaction metric
        auto* satTip = new QLabel(
            "Satisfaction = (completed days − stalled days × 0.5) ÷ total days × 100\n"
            "Grade:  S ≥ 90  ·  A ≥ 75  ·  B ≥ 60  ·  C ≥ 45  ·  D ≥ 30  ·  E < 30");
        satTip->setWordWrap(true);
        satTip->setStyleSheet(QString(
            "color: %1; font-size: 11px; background: %2;"
            " border: 1px solid %3; border-left: 3px solid %4;"
            " border-radius: 8px; padding: 8px 12px;")
            .arg(Cl::ink2).arg(Cl::blueBg).arg(Cl::blueBdr).arg(Cl::blue));
        satCard->layout()->addWidget(satTip);

        // Horizontal bar per plan
        auto* satChart = new BarChartWidget(BarChartWidget::Horizontal);
        int satH = std::max(80, int(planRows.size()) * 42 + 24);
        satChart->setFixedHeight(satH);
        satChart->setMinimumWidth(300);
        QVector<BarData> satData;

        // Sort by satisfaction descending for this chart
        QVector<int> satOrder(planRows.size());
        std::iota(satOrder.begin(), satOrder.end(), 0);
        std::sort(satOrder.begin(), satOrder.end(), [&](int a, int b){
            return planRows[a].satisfactionScore > planRows[b].satisfactionScore;
        });

        for (int idx : satOrder) {
            const auto& r = planRows[idx];
            auto [sg, sc] = satGrade(r.satisfactionScore);
            QString lbl = r.name.length() > 16 ? r.name.left(14) + "…" : r.name;
            satData.append({lbl + "  " + sg, double(r.satisfactionScore), sc});
        }
        satChart->setData(satData);
        satCard->layout()->addWidget(satChart);
        m_contentLayout->addWidget(satCard);

        // ── Section 6: Milestones overview ───────────────────────
        if (milestonesAll > 0) {
            m_contentLayout->addWidget(makeSectionLabel("Milestone Overview"));
            auto* msCard = makeCard();
            auto* msRow  = new QHBoxLayout;

            auto* msRing = new RingChartWidget;
            msRing->setMinimumSize(110, 110);
            msRing->setMaximumSize(160, 160);
            msRing->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            int msRemaining = milestonesAll - milestonesCleared;
            msRing->setData({
                {"Cleared",   double(milestonesCleared), Cl::green},
                {"Remaining", double(msRemaining),       Cl::cream4},
            }, QString("%1 / %2").arg(milestonesCleared).arg(milestonesAll));

            auto* msDetails = new QVBoxLayout;
            msDetails->setSpacing(8);
            struct MR { QString c, lbl; int val; };
            QVector<MR> mrows = {
                {Cl::green,  "Milestones cleared:",  milestonesCleared},
                {Cl::cream4, "Still to reach:",       msRemaining},
                {Cl::gold2,  "Total milestones:",     milestonesAll},
                {Cl::purple, "Clear rate:",
                 int(float(milestonesCleared)/std::max(1,milestonesAll)*100)},
            };
            bool lastIsPercent = true;
            for (int i = 0; i < mrows.size(); i++) {
                const auto& mr = mrows[i];
                auto* rw = new QHBoxLayout;
                auto* d2 = new QLabel; d2->setFixedSize(10,10);
                d2->setStyleSheet(QString("background:%1;border-radius:2px;border:none;").arg(mr.c));
                auto* ll = new QLabel(mr.lbl);
                ll->setStyleSheet(QString("color:%1;font-size:11px;border:none;background:transparent;").arg(Cl::ink3));
                auto* vv = new QLabel(i==3
                    ? QString("%1%").arg(mr.val)
                    : QString::number(mr.val));
                vv->setStyleSheet(QString("color:%1;font-size:12px;font-weight:600;border:none;background:transparent;").arg(Cl::ink));
                rw->addWidget(d2); rw->addWidget(ll); rw->addStretch(); rw->addWidget(vv);
                msDetails->addLayout(rw);
            }
            msDetails->addStretch();

            msRow->addWidget(msRing);
            msRow->addSpacing(20);
            msRow->addLayout(msDetails, 1);
            msCard->layout()->addLayout(msRow);
            m_contentLayout->addWidget(msCard);
        }

        m_contentLayout->addStretch();
    }

    // ── Build the full content area ───────────────────────────────
    void buildContent() {
        clearContent();

        QString planId = m_planCombo->currentData().toString();

        if (planId == "__overall__") {
            buildOverallContent();
            return;
        }

        auto* plan = DataManager::instance().planById(planId);
        if (!plan) {
            auto* empty = new QLabel("No plan selected. Create a quest line in the Setup tab.");
            empty->setAlignment(Qt::AlignCenter);
            empty->setStyleSheet(QString("color: %1; font-size: 14px; border: none;").arg(Cl::ink4));
            m_contentLayout->addStretch();
            m_contentLayout->addWidget(empty);
            m_contentLayout->addStretch();
            return;
        }

        const Progress& prog = DataManager::instance().progressFor(planId);
        Stats s = computeStats(*plan, prog);

        // ── Section 1: Key metric tiles ───────────────────────────
        auto* tilesLabel = makeSectionLabel("Key Metrics");
        m_contentLayout->addWidget(tilesLabel);

        auto* tilesGrid = new QGridLayout;
        tilesGrid->setSpacing(12);
        for (int col = 0; col < 4; col++)
            tilesGrid->setColumnStretch(col, 1);  // all 4 columns grow equally

        QString paceStr, paceColor;
        if (s.daysElapsed == 0) {
            paceStr  = "Just started";
            paceColor = Cl::ink3;
        } else if (s.paceRatio >= 1.0f) {
            paceStr  = QString("Ahead by %1%").arg(int((s.paceRatio - 1.0f) * s.expectedPct));
            paceColor = Cl::green2;
        } else {
            paceStr  = QString("Behind by %1%").arg(int((1.0f - s.paceRatio) * s.expectedPct));
            paceColor = Cl::red;
        }

        struct TileSpec { QString icon, val, lbl, sub, color; };
        QVector<TileSpec> tiles = {
            {"✅", QString::number(s.completionPct)+"%", "Completion",
             QString("%1 / %2 days done").arg(s.doneDays).arg(s.totalDays), Cl::purple},
            {"⚡", QString::number(s.totalXp), "Total XP",
             QString("Avg %1 XP/day completed").arg(int(s.avgXpPerDay)), Cl::purple2},
            {"🔥", QString::number(s.streakCurrent), "Current Streak",
             QString("Best ever: %1 days").arg(s.streakBest), Cl::green},
            {"📈", paceStr, "Pace",
             QString("Expected %1% done by now").arg(int(s.expectedPct)), paceColor},
            {"🏆", QString("%1 / %2").arg(s.milestonesCleared).arg(s.milestonesTotal),
             "Milestones", QString("%1 XP from checkpoints").arg(s.milestoneXpTotal), Cl::gold2},
            {"⏸",  QString::number(s.stalledDays_), "Stalled Days",
             QString("Longest stall run: %1 day%2").arg(s.maxStallRun).arg(s.maxStallRun==1?"":"s"), Cl::gold2},
            {"📅", s.projectedFinishDate.isValid()
                    ? s.projectedFinishDate.toString("MMM d, yyyy") : "—",
             "Projected Finish",
             s.paceRatio >= 1.0f ? "On track" : "Slipping behind", Cl::blue},
            {"⬛", QString::number(s.remainingDays), "Days Remaining",
             QString("%1 days ahead of today").arg(s.futureDays), Cl::ink3},
        };

        for (int i = 0; i < tiles.size(); i++) {
            const auto& t = tiles[i];
            auto* tile = new StatTile(t.icon, t.val, t.lbl, t.sub, t.color);
            tilesGrid->addWidget(tile, i / 4, i % 4);
        }
        auto* tilesContainer = new QWidget;
        tilesContainer->setStyleSheet("background: transparent; border: none;");
        tilesContainer->setLayout(tilesGrid);
        m_contentLayout->addWidget(tilesContainer);

        // ── Section 2: XP Rank card ───────────────────────────────
        m_contentLayout->addWidget(makeSectionLabel("Rank Progression"));
        auto* rankCard = makeCard();
        auto* rcl = rankCard->layout();

        auto* rankRow = new QHBoxLayout;
        auto* rankBadge = new QLabel(QString("  %1  ").arg(s.currentRankLabel));
        rankBadge->setStyleSheet(QString("color: %1; border: 2px solid %1;"
            " border-radius: 7px; font-size: 13px; font-weight: 700;"
            " letter-spacing: 2px; padding: 5px 14px; background: %2;")
            .arg(s.currentRankColor).arg(Cl::white_));
        rankRow->addWidget(rankBadge); rankRow->addStretch();
        if (s.xpToNextRank > 0) {
            auto* nextLbl = new QLabel(QString("%1 XP to  %2")
                .arg(s.xpToNextRank).arg(s.nextRankLabel));
            nextLbl->setStyleSheet(QString("color: %1; font-size: 11px;"
                " border: none; background: transparent;").arg(Cl::ink4));
            rankRow->addWidget(nextLbl);
        }
        rcl->addLayout(rankRow);

        auto* xpBarLbl = new QHBoxLayout;
        auto* xpL = new QLabel(QString("%1 XP").arg(s.totalXp));
        xpL->setStyleSheet("border: none; background: transparent; font-weight: 600;");
        auto* xpR = new QLabel(QString("%1%  in current rank").arg(s.rankPct));
        xpR->setStyleSheet(QString("color: %1; font-size: 11px; border: none; background: transparent;").arg(Cl::ink4));
        xpBarLbl->addWidget(xpL); xpBarLbl->addStretch(); xpBarLbl->addWidget(xpR);
        rcl->addLayout(xpBarLbl);

        auto* xpBar = new XpBarWidget;
        xpBar->setFixedHeight(9); xpBar->setPct(s.rankPct);
        xpBar->setColor(Cl::blue, Cl::teal);
        rcl->addWidget(xpBar);

        // Rank full table
        auto* rankTable = new QWidget;
        rankTable->setStyleSheet("background: transparent; border: none;");
        auto* rtl = new QHBoxLayout(rankTable);
        rtl->setContentsMargins(0, 8, 0, 0); rtl->setSpacing(0);
        auto ranks = Util::effectiveRanks(*plan);
        for (const auto& r : ranks) {
            bool unlocked = s.totalXp >= r.xp;
            auto* col = new QVBoxLayout;
            auto* badge = new QLabel(r.rank);
            badge->setAlignment(Qt::AlignCenter);
            badge->setStyleSheet(QString("color: %1; border: 2px solid %1;"
                " border-radius: 5px; font-size: 10px; font-weight: 700;"
                " padding: 2px 6px; background: %2;")
                .arg(unlocked ? r.color : Cl::ink4)
                .arg(Cl::white_));
            auto* xpLbl = new QLabel(QString("%1").arg(r.xp));
            xpLbl->setAlignment(Qt::AlignCenter);
            xpLbl->setStyleSheet(QString("color: %1; font-size: 9px; border: none; background: transparent;").arg(Cl::ink4));
            col->addWidget(badge); col->addWidget(xpLbl);
            rtl->addLayout(col);
            if (&r != &ranks.last()) {
                auto* arrow = new QLabel("→");
                arrow->setAlignment(Qt::AlignCenter);
                arrow->setStyleSheet(QString("color: %1; font-size: 11px; border: none; background: transparent;").arg(Cl::ink4));
                rtl->addWidget(arrow);
            }
        }
        rcl->addWidget(rankTable);
        m_contentLayout->addWidget(rankCard);

        // ── Section 3: Activity calendar ─────────────────────────
        m_contentLayout->addWidget(makeSectionLabel("Activity Calendar (Last 16 Weeks)"));
        auto* calCard = makeCard();
        auto* cal = new WeeklyCalendarWidget;
        cal->setData(prog.completedDays, prog.stalledDays, plan->startDate);

        // Legend row
        auto* calLegRow = new QHBoxLayout;
        struct LE { QString c, l; };
        for (const LE& le : QVector<LE>{{Cl::green,"Done"},{Cl::gold2,"Stalled"},{Cl::cardBg3,"Pending"},{Cl::cardBg2,"Not in plan"}}) {
            auto* dot = new QLabel;
            dot->setFixedSize(12,12);
            dot->setStyleSheet(QString("background:%1;border-radius:2px;border:1px solid %2;").arg(le.c).arg(Cl::border));
            auto* ll = new QLabel(le.l);
            ll->setStyleSheet(QString("color:%1;font-size:10px;border:none;").arg(Cl::ink4));
            calLegRow->addWidget(dot); calLegRow->addWidget(ll); calLegRow->addSpacing(12);
        }
        calLegRow->addStretch();
        calCard->layout()->addLayout(calLegRow);
        calCard->layout()->addWidget(cal);
        m_contentLayout->addWidget(calCard);

        // ── Section 4: Weekly completion rate chart ───────────────
        m_contentLayout->addWidget(makeSectionLabel("Weekly Completion Rate (Last 8 Weeks)"));
        auto* wkCard = makeCard();
        auto* wkChart = new BarChartWidget(BarChartWidget::Vertical);
        wkChart->setMinimumHeight(160);
        wkChart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QVector<BarData> wkData;
        QDate today = QDate::currentDate();
        for (int i = 0; i < 8; i++) {
            QDate ws = today.addDays(-(7 - i) * 7);
            QString lbl = ws.toString("MMM d");
            QString col = s.weeklyRate[i] >= 70 ? Cl::teal
                        : s.weeklyRate[i] >= 40 ? Cl::blue : Cl::gold;
            wkData.append({lbl, s.weeklyRate[i], col});
        }
        wkChart->setData(wkData, "% days completed");
        wkCard->layout()->addWidget(wkChart);
        m_contentLayout->addWidget(wkCard);

        // ── Section 5: Day-of-week distribution ───────────────────
        m_contentLayout->addWidget(makeSectionLabel("Best Days of the Week"));
        auto* dowCard = makeCard();
        auto* dowRow = new QHBoxLayout;

        // Bar chart (vertical)
        auto* dowChart = new BarChartWidget(BarChartWidget::Vertical);
        dowChart->setMinimumHeight(140);
        dowChart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QStringList dowNames = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
        QVector<BarData> dowData;
        int maxDow = *std::max_element(s.dowCounts.begin(), s.dowCounts.end());
        for (int i = 0; i < 7; i++) {
            QString col = s.dowCounts[i] == maxDow && maxDow > 0 ? Cl::teal : Cl::blue;
            dowData.append({dowNames[i], double(s.dowCounts[i]), col});
        }
        dowChart->setData(dowData);

        // Ring chart (weekday vs weekend)
        int weekdayCount = 0, weekendCount = 0;
        for (int i = 0; i < 5; i++) weekdayCount += s.dowCounts[i];
        for (int i = 5; i < 7; i++) weekendCount += s.dowCounts[i];
        auto* ringChart = new RingChartWidget;
        ringChart->setMinimumSize(120, 120);
        ringChart->setMaximumSize(180, 180);
        ringChart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        ringChart->setData({
            {"Weekdays", double(weekdayCount), Cl::blue},
            {"Weekend",  double(weekendCount), Cl::teal},
        }, QString("%1\ndays").arg(s.doneDays));

        auto* ringLegend = new QVBoxLayout;
        ringLegend->setSpacing(6);
        ringLegend->addStretch();
        for (const auto& seg : QVector<QPair<QString,QString>>{
                {Cl::purple, QString("Weekdays: %1").arg(weekdayCount)},
                {Cl::green,  QString("Weekend:  %1").arg(weekendCount)}}) {
            auto* lr = new QHBoxLayout;
            auto* dot = new QLabel; dot->setFixedSize(11,11);
            dot->setStyleSheet(QString("background:%1;border-radius:2px;border:none;").arg(seg.first));
            auto* ll = new QLabel(seg.second);
            ll->setStyleSheet(QString("color:%1;font-size:11px;border:none;background:transparent;").arg(Cl::ink3));
            lr->addWidget(dot); lr->addWidget(ll); lr->addStretch();
            ringLegend->addLayout(lr);
        }
        ringLegend->addStretch();

        dowRow->addWidget(dowChart, 3);
        dowRow->addSpacing(16);
        dowRow->addWidget(ringChart);
        dowRow->addLayout(ringLegend);
        dowCard->layout()->addLayout(dowRow);
        m_contentLayout->addWidget(dowCard);

        // ── Section 6: Phase / milestone breakdown ─────────────────
        if (!s.phases.isEmpty()) {
            m_contentLayout->addWidget(makeSectionLabel("Progress by Phase"));
            auto* phCard = makeCard();
            auto* phChart = new BarChartWidget(BarChartWidget::Horizontal);
            phChart->setFixedHeight(std::max(140, int(s.phases.size()) * 42 + 24));
            phChart->setMinimumWidth(380);
            QVector<BarData> phData;
            for (const auto& ph : s.phases) {
                int pct2 = ph.days > 0 ? int(double(ph.done) / ph.days * 100) : 0;
                QString col = pct2 >= 100 ? Cl::teal : pct2 >= 50 ? Cl::blue : Cl::purple;
                QString label = ph.name.length() > 18 ? ph.name.left(16) + "…" : ph.name;
                phData.append({label, double(pct2), col});
            }
            phChart->setData(phData, "% done");
            phCard->layout()->addWidget(phChart);

            // Phase table detail
            auto* phTable = new QTableWidget(s.phases.size(), 5);
            phTable->setHorizontalHeaderLabels({"Phase","Days","Done","XP Earned","XP Total"});
            phTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
            phTable->verticalHeader()->setVisible(false);
            phTable->setAlternatingRowColors(true);
            phTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
            phTable->setStyleSheet(phTable->styleSheet() +
                QString("QTableWidget{alternate-background-color:%1;}").arg(Cl::cardBg2));

            for (int i = 0; i < s.phases.size(); i++) {
                const auto& ph = s.phases[i];
                auto item = [](const QString& t, Qt::AlignmentFlag a = Qt::AlignLeft) {
                    auto* it = new QTableWidgetItem(t);
                    it->setFlags(Qt::ItemIsEnabled);
                    it->setTextAlignment(a | Qt::AlignVCenter);
                    return it;
                };
                phTable->setItem(i, 0, item(ph.name));
                phTable->setItem(i, 1, item(QString::number(ph.days), Qt::AlignCenter));
                phTable->setItem(i, 2, item(QString("%1 / %2").arg(ph.done).arg(ph.days), Qt::AlignCenter));
                phTable->setItem(i, 3, item(QString::number(ph.xpEarned), Qt::AlignCenter));
                phTable->setItem(i, 4, item(QString::number(ph.xpTotal), Qt::AlignCenter));
                phTable->setRowHeight(i, 32);
                if (ph.done == ph.days && ph.days > 0)
                    for (int c = 0; c < 5; c++)
                        phTable->item(i,c)->setForeground(QColor(Cl::green2));
            }
            int tableH = std::min(int(s.phases.size()) * 32 + 38, 240);
            phTable->setFixedHeight(tableH);
            phCard->layout()->addWidget(phTable);
            m_contentLayout->addWidget(phCard);
        }

        // ── Section 7: XP Sources breakdown ──────────────────────
        m_contentLayout->addWidget(makeSectionLabel("XP Breakdown"));
        auto* xpCard = makeCard();
        auto* xpRow = new QHBoxLayout;

        int normalXp = s.xpFromCompleted - s.milestoneXpTotal;
        auto* xpRing = new RingChartWidget;
        xpRing->setMinimumSize(110, 110);
        xpRing->setMaximumSize(160, 160);
        xpRing->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        if (s.totalXp > 0) {
            xpRing->setData({
                {"Normal",     double(normalXp),          Cl::blue},
                {"Milestones", double(s.milestoneXpTotal), Cl::gold2},
            }, QString("%1\nXP").arg(s.totalXp));
        }

        auto* xpDetails = new QVBoxLayout;
        xpDetails->setSpacing(8);
        struct XR { QString c, label; int val; };
        QVector<XR> xpRows = {
            {Cl::purple,  "From normal quests:",  normalXp},
            {Cl::gold2,   "From milestones:",     s.milestoneXpTotal},
            {Cl::ink4,    "Still earnable:",       s.xpPossibleRemaining},
            {Cl::ink4,    "Avg XP/completed day:", int(s.avgXpPerDay)},
        };
        for (const auto& xr : xpRows) {
            auto* row = new QHBoxLayout;
            auto* dot = new QLabel; dot->setFixedSize(10,10);
            dot->setStyleSheet(QString("background:%1;border-radius:2px;border:none;").arg(xr.c));
            auto* ll = new QLabel(xr.label);
            ll->setStyleSheet(QString("color:%1;font-size:11px;border:none;background:transparent;").arg(Cl::ink3));
            auto* vv = new QLabel(QString::number(xr.val));
            vv->setStyleSheet(QString("color:%1;font-size:12px;font-weight:600;border:none;background:transparent;").arg(Cl::ink));
            row->addWidget(dot); row->addWidget(ll); row->addStretch(); row->addWidget(vv);
            xpDetails->addLayout(row);
        }
        xpDetails->addStretch();

        xpRow->addWidget(xpRing);
        xpRow->addSpacing(20);
        xpRow->addLayout(xpDetails, 1);
        xpCard->layout()->addLayout(xpRow);
        m_contentLayout->addWidget(xpCard);

        // ── Section 8: Notable days ───────────────────────────────
        m_contentLayout->addWidget(makeSectionLabel("Notable Days"));
        auto* notCard = makeCard();
        auto* notGrid = new QGridLayout;
        notGrid->setSpacing(10);

        struct Notable { QString icon, title, sub, color; };
        QVector<Notable> notables = {
            {"🏆", "Highest XP day",
             s.highestXpDay > 0 ? QString("Day %1 — %2").arg(s.highestXpDay).arg(s.highestXpTitle) : "N/A",
             Cl::gold2},
            {"⚡", "Current streak",
             QString("%1 consecutive days  (best: %2)").arg(s.streakCurrent).arg(s.streakBest),
             Cl::green},
            {"📉", "Longest missed gap",
             s.longestGap > 0 ? QString("%1 days between completions").arg(s.longestGap) : "No gaps yet",
             Cl::red},
            {"⏱",  "Avg days between sessions",
             s.avgDaysBetween > 0 ? QString("%1 day%2").arg(s.avgDaysBetween).arg(s.avgDaysBetween==1?"":"s") : "N/A",
             Cl::blue},
        };
        for (int i = 0; i < notables.size(); i++) {
            const auto& n = notables[i];
            auto* tile = new QFrame;
            tile->setStyleSheet(QString("QFrame{background:%1;border:1px solid %2;border-radius:12px;}")
                .arg(Cl::cardBg2).arg(Cl::border));
            auto* tl = new QVBoxLayout(tile);
            tl->setContentsMargins(14,12,14,12); tl->setSpacing(3);
            auto* iconL = new QLabel(n.icon);
            iconL->setStyleSheet("font-size:18px;border:none;background:transparent;");
            auto* titL = new QLabel(n.title);
            titL->setStyleSheet(QString("color:%1;font-size:10px;font-weight:700;"
                " letter-spacing:1px;border:none;background:transparent;").arg(Cl::ink4));
            auto* subL = new QLabel(n.sub);
            subL->setStyleSheet(QString("color:%1;font-size:12px;font-weight:600;"
                "border:none;background:transparent;").arg(n.color));
            subL->setWordWrap(true);
            tl->addWidget(iconL); tl->addWidget(titL); tl->addWidget(subL);
            notGrid->addWidget(tile, i/2, i%2);
        }
        notCard->layout()->addLayout(notGrid);
        m_contentLayout->addWidget(notCard);

        m_contentLayout->addStretch();
    }

    // ── Helpers ───────────────────────────────────────────────────
    QLabel* makeSectionLabel(const QString& text) {
        auto* lbl = new QLabel(text.toUpper());
        lbl->setStyleSheet(QString("color: %1; font-size: 10px; font-weight: 700;"
            " letter-spacing: 3px; border: none; background: transparent;"
            " padding: 6px 0 3px 0;").arg(Cl::navy));
        return lbl;
    }

    struct CardWidget : public QFrame {
        QVBoxLayout* m_inner;
        CardWidget(QWidget* parent = nullptr) : QFrame(parent) {
            setStyleSheet(QString("QFrame{background:%1;border:1px solid %2;border-radius:14px;}")
                          .arg(Cl::white_).arg(Cl::border));
            m_inner = new QVBoxLayout(this);
            m_inner->setContentsMargins(20,16,20,16);
            m_inner->setSpacing(10);
        }
        QVBoxLayout* layout() { return m_inner; }
    };

    CardWidget* makeCard() { return new CardWidget; }
};

// ============================================================
//  MAIN WINDOW
// ============================================================
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow() {
        setWindowTitle("reSTEM");
        setMinimumSize(800, 560);

        // Start at 85% of available screen, capped at 1400×900
        QScreen* scr = QApplication::primaryScreen();
        QSize avail = scr ? scr->availableSize() : QSize(1280, 800);
        int w = std::min(int(avail.width()  * 0.85), 1400);
        int h = std::min(int(avail.height() * 0.85), 900);
        resize(std::max(w, 900), std::max(h, 600));

        auto* central = new QWidget(this);
        setCentralWidget(central);
        auto* root = new QVBoxLayout(central);
        root->setContentsMargins(0,0,0,0); root->setSpacing(0);

        m_tabs = new QTabWidget;
        m_tabs->setDocumentMode(true);

        m_dashboard = new DashboardTab;
        m_setup     = new SetupTab;
        m_stats     = new StatsTab;
        m_manage    = new ManageTab;

        m_tabs->addTab(m_dashboard, " Dashboard ");
        m_tabs->addTab(m_setup,     " Setup ");
        m_tabs->addTab(m_stats,     " Statistics ");
        m_tabs->addTab(m_manage,    " Manage ");
        // SVG tab icons — sized 18px, tinted to navy for normal, blue for active
        m_tabs->setTabIcon(0, svgIcon("xp",     18, QColor(Cl::blue)));
        m_tabs->setTabIcon(1, svgIcon("list",   18, QColor(Cl::blue)));
        m_tabs->setTabIcon(2, svgIcon("stats",  18, QColor(Cl::blue)));
        m_tabs->setTabIcon(3, svgIcon("gears",  18, QColor(Cl::blue)));
        m_tabs->setIconSize(QSize(18, 18));

        connect(m_dashboard, &DashboardTab::newQuestLineRequested, this, [this]{
            m_tabs->setCurrentWidget(m_setup);
        });
        connect(m_setup, &SetupTab::planChanged,    this, &MainWindow::refreshAll);
        connect(m_manage, &ManageTab::planChanged,  this, &MainWindow::refreshAll);
        connect(m_manage, &ManageTab::editPlanRequested, this, [this]{
            m_tabs->setCurrentWidget(m_setup);
            m_setup->refresh();
        });

        root->addWidget(m_tabs);
    }

    void refreshAll() {
        m_dashboard->refresh();
        m_setup->refresh();
        m_stats->refresh();
        m_manage->refresh();
    }

private:
    QTabWidget*   m_tabs      = nullptr;
    DashboardTab* m_dashboard = nullptr;
    SetupTab*     m_setup     = nullptr;
    StatsTab*     m_stats     = nullptr;
    ManageTab*    m_manage    = nullptr;
};

// ============================================================
//  ENTRY POINT
// ============================================================
int main(int argc, char* argv[])
{
    // Enable HiDPI / Retina
    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication app(argc, argv);
    app.setApplicationName("reSTEM");
    app.setOrganizationName("reSTEM");

    // Fusion style bypasses the native OS theme engine entirely.
    // This prevents Windows dark mode from giving dialogs a dark
    // background with black text (unreadable). Fusion uses only
    // the explicit palette below on all platforms.
    app.setStyle("Fusion");

    // Explicit light palette — covers every role that QMessageBox
    // and QDialog inspect. Works identically on Windows, macOS, Linux.
    QPalette pal;
    pal.setColor(QPalette::Window,          QColor("#F7F8FA"));
    pal.setColor(QPalette::WindowText,      QColor("#21242C"));
    pal.setColor(QPalette::Base,            QColor("#FFFFFF"));
    pal.setColor(QPalette::AlternateBase,   QColor("#F2F4F8"));
    pal.setColor(QPalette::Text,            QColor("#21242C"));
    pal.setColor(QPalette::BrightText,      QColor("#21242C"));
    pal.setColor(QPalette::ButtonText,      QColor("#21242C"));
    pal.setColor(QPalette::Button,          QColor("#FFFFFF"));
    pal.setColor(QPalette::Highlight,       QColor("#1865F2"));
    pal.setColor(QPalette::HighlightedText, QColor("#FFFFFF"));
    pal.setColor(QPalette::ToolTipBase,     QColor("#21242C"));
    pal.setColor(QPalette::ToolTipText,     QColor("#FFFFFF"));
    pal.setColor(QPalette::Link,            QColor("#1865F2"));
    pal.setColor(QPalette::LinkVisited,     QColor("#7B439E"));
    app.setPalette(pal);

    app.setStyleSheet(buildQSS());

    // Font selection: prefer system sans-serif
    QFont appFont;
#if defined(Q_OS_MAC)
    appFont.setFamily("SF Pro Text");
    if (!appFont.exactMatch()) appFont.setFamily("Helvetica Neue");
#elif defined(Q_OS_WIN)
    appFont.setFamily("Segoe UI");
#elif defined(Q_OS_LINUX)
    appFont.setFamily("Noto Sans");
    if (!appFont.exactMatch()) appFont.setFamily("Liberation Sans");
    if (!appFont.exactMatch()) appFont.setFamily("DejaVu Sans");
#endif
    appFont.setPointSize(11);
    app.setFont(appFont);

    DataManager::instance().load();

    MainWindow win;
    win.refreshAll();
    win.show();

    return app.exec();
}

// Required for Q_OBJECT in .cpp files — CMake AUTOMOC generates this
#include "main.moc"