#include "log.h"

void clutchlog_config()
{
    using level = clutchlog::level;
    using fmt   = clutchlog::fmt;
    using fg    = clutchlog::fmt::fg;
    using bg    = clutchlog::fmt::bg;
    using typo  = clutchlog::fmt::typo;
    auto& log = clutchlog::logger();

    log.out(std::cerr);
    log.hfill_min(0);
    log.hfill_max(200);
    log.depth_mark(" |");

    log.style(level::critical,
              fg::black,
              bg::red,
              typo::bold);
    log.style(level::error,
              fg::black,
              bg::red);
    log.style(level::warning,
              fg::black,
              bg::magenta,
              typo::bold);
    log.style(level::progress,
              fg::yellow);
    log.style(level::note,
              fg::green);
    log.style(level::info,
              fg::magenta);
    log.style(level::debug,
              fg::cyan);
    log.style(level::xdebug,
              fg::blue);

    std::vector<clutchlog::fmt> greys = {fmt(15)};
    for(unsigned short i=239; i>235; i-=1) {
        greys.push_back( fmt(i) );
    }
    log.depth_styles(greys);

    log.filehash_styles(
        {fmt(19),fmt(21),fmt(24),fmt(37),
         fmt(55),fmt(57),fmt(61),fmt(63),
         fmt(91),fmt(93),fmt(97),fmt(99)} );

    log.funchash_styles(
        {fmt(130),fmt(136),fmt(142),
         fmt(166),fmt(172),fmt(178),
         fmt(202),fmt(208),fmt(214),
         fmt(203),fmt(209),fmt(215),
         fmt(204),fmt(210),fmt(216)} );

    std::ostringstream format;
    fmt reset(typo::reset);
    fmt bold(typo::bold);
    #ifndef NDEBUG
        fmt discreet(fg::black);
        format
               // << fmt(typo::inverse)
               << "{level_fmt}"
               << "{level_short}" << reset
               << bold << "{funchash_fmt} |" << reset
               << "{depth_marks} "
               << "{level_fmt}" << bold("{msg}")
               << discreet(" {hfill} ")
               << "{funchash_fmt}{func}"
               << discreet(" @ ") << reset
               << "{filehash_fmt}{file}"
               << discreet(":")
               << "{filehash_fmt}{line}"
               << "\n";
        log.format(format.str());
    #else
        format << "[{name}] {level_fmt}{level_letter}" << reset
               << ":{depth_marks} "
               << bold("{level_fmt}{msg}") << "\n";
        log.format(format.str());
    #endif

}

