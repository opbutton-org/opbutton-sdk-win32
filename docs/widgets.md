# Widgets

## Retained layout options

Elements receive generated IDs automatically. Layout edits preserve IDs; removing
or replacing a tree detaches its callbacks and providers. Serialize tree edits on
the host UI thread. The SDK ID counter itself is thread-safe.

Rows and columns support gap, padding, align, justify, wrap and inherited density.
Use a grid for equal-width commands: columns is the maximum column count, and
minCellWidth allows automatic reduction on narrow panels. Placement supports
content width, numeric width, fill with a weight, minimum/maximum width and
alignSelf. Dimensions are dashboard CSS pixels. Radio options stay in one group;
slider changes are sent on release/keyboard commit, with a numeric value.

Install the updated backend and dashboard before using these extensions. Protocol
version 2 remains compatible with existing panels. Optional public ID aliases
retain their existing meaning and are unnecessary for creating elements.

```cpp
#include "OpButton/OpButtonUI.h"
#include "OpButton/OpButtonTab.h"
#include "OpButton/OpButtonButton.h"
#include "OpButton/OpButtonSelect.h"
#include "OpButton/OpButtonRadioGroup.h"
#include "OpButton/OpButtonSlider.h"

auto & root = ui.addTab("Game").vertical().gap(12).padding(8);
auto & commands = root.addGrid().columns(3).minCellWidth(140).gap(8);
commands.addButton("Restart");
commands.addButton("Clear logs");
auto & row = root.addHorizontal().gap(8).align("end");
row.addSelect("Language").options({{"en", "English"}, {"ru", "Русский"}})
    .defaultValue("en").fill().minWidth(160);
row.addButton("Apply").width(120);
root.addRadioGroup("Difficulty").options({{"easy", "Easy"}, {"hard", "Hard"}})
    .defaultValue("easy").optionDirection("horizontal");
auto & speed = root.addSlider("Speed").range(0.5, 3, 0.1).defaultValue(1).unit("x");
speed.fill();
speed.onChange(OpButton::OpButtonFunction<void(const OpButton::OpButtonSliderAction &)>(
    [](const OpButton::OpButtonSliderAction & action) { setSpeed(action.value()); }));
```

Shared placement setters return the element base. Keep a widget reference when
configuring both placement and typed callbacks. Returned references remain
borrowed and expire when their owning tree is removed or replaced.
