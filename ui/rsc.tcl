package require Iwidgets 3.0

# itkwish interprets the rest...
# ----------------------------------------------------------------------
label .status -textvariable statusVar -width 40 -anchor w
pack .status -side bottom

iwidgets::toolbar .tb -helpvariable statusVar -orient vertical
pack .tb -side left -anchor nw -padx 4 -pady 4

set imagedir [file join ${iwidgets::library} demos images]

.tb add button select \
    -helpstr "Select drawing elements" \
    -image [image create photo -file [file join $imagedir select.gif]] \
    -balloonstr "Selection tool" \
    -command {puts "tool: select"}

.tb add button magnify \
    -helpstr "Magnify drawing area" \
    -image [image create photo -file [file join $imagedir mag.gif]] \
    -balloonstr "Zoom tool" \
    -command {puts "tool: magnify"}

.tb add button ruler \
    -helpstr "Measure distances on drawing" \
    -image [image create photo -file [file join $imagedir ruler.gif]] \
    -balloonstr "Ruler tool" \
    -command {puts "tool: ruler"}

.tb add frame filler \
    -borderwidth 1 \
    -width 10 \
    -height 10

.tb add button poly \
    -helpstr "Draw a polygon" \
    -image [image create photo -file [file join $imagedir poly.gif]] \
    -balloonstr "Polygon tool" \
    -command {puts "tool: polygon"}

canvas .worksp -width 2i -height 3i \
    -borderwidth 2 -relief sunken -background white
pack .worksp -side right -expand yes -fill both -padx 4 -pady 4

