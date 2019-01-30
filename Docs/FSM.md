
# FSM Design

Using library: https://github.com/tkem/fsmlite

## States (enum)

    is_paused
    is_running

## Actions (verbs) and Callbacks (functions)

    draw
    close
    minimize
    paused

## Transitions (Start (a state), Event, Target (a state), Action (a callback))

    mouse_clicked
    mouse_moved
