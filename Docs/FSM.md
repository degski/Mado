
# FSM Design

Using library: https://github.com/tkem/fsmlite

## States (enum)

    is_paused
    is_running


## Transitions (Start (a state), Event, Target (a state), Action (a callback))

### Events

    mouse_clicked
    mouse_moved
    lost_focus
    gained_focus


## Actions (verbs) and Callbacks (functions)

    draw
    close
    minimize
    pause
