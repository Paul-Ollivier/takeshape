# SPDX-License-Identifier: AGPL-3.0-or-later
# JUCE module sources are compiled into our plugin target, so we use JUCE's own recommended
# warning set rather than piling on -Wconversion etc. (which would light up JUCE internals).
# Stricter warnings live on the pure-C++ core library, which never includes JUCE module sources.
function(restyle_set_strict_warnings target)
    target_compile_options(${target} PRIVATE
        -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor -Wconversion -Woverloaded-virtual)
endfunction()
