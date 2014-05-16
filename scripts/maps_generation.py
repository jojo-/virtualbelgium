#!/usr/bin/env python

##################################################################
#
# Map rendering script for VirtualBelgium
#
# See https://github.com/mapnik/mapnik/wiki/GettingStartedInPython
# for referecences and indications
#
# Requires : mapnik2, cairo and subprocess libraries
#            ffmpeg for generating a movie
#
# Version  : 31 mai 2013
# Author   : J. Barthelemy
# 
##################################################################

# Importing required libraries

import mapnik
import cairo
from subprocess import call

# Listing time step intervals

list_H = ["[H0]", "[H1]", "[H2]", "[H3]", "[H4]", "[H5]", "[H6]", "[H7]", "[H8]", "[H9]", "[H10]",
          "[H11]", "[H12]", "[H13]", "[H14]", "[H15]", "[H16]", "[H17]", "[H18]", "[H19]", "[H20]",
          "[H21]", "[H22]", "[H23]"]

# Main loop over the time steps

i = 0;
for HH in list_H:
    print "rendering image " + HH + "..."

    # Creating a map

    m = mapnik.Map(1200,600)                                           # size of the map in pixel
    m.background = mapnik.Color('white')                               # background color

    # Creating a style

    s   = mapnik.Style()                                               # style object to hold rules
    r1  = mapnik.Rule()
    r2  = mapnik.Rule()
    r3  = mapnik.Rule()
    r4  = mapnik.Rule()
    r5  = mapnik.Rule()
    r6  = mapnik.Rule()
    r7  = mapnik.Rule()
    r8  = mapnik.Rule()
    r9  = mapnik.Rule()
    r10 = mapnik.Rule()

    
    p1_s  = mapnik.PolygonSymbolizer(mapnik.Color('#F7FBFF'))          # to fill a polygon we create a PolygonSymbolizer
    p2_s  = mapnik.PolygonSymbolizer(mapnik.Color('#E1EDF8'))
    p3_s  = mapnik.PolygonSymbolizer(mapnik.Color('#CCDFF1'))
    p4_s  = mapnik.PolygonSymbolizer(mapnik.Color('#AFD1E7'))
    p5_s  = mapnik.PolygonSymbolizer(mapnik.Color('#88BEDC'))
    p6_s  = mapnik.PolygonSymbolizer(mapnik.Color('#5FA6D1'))
    p7_s  = mapnik.PolygonSymbolizer(mapnik.Color('#3D8DC3'))
    p8_s  = mapnik.PolygonSymbolizer(mapnik.Color('#2171B5'))
    p9_s  = mapnik.PolygonSymbolizer(mapnik.Color('#0A539E'))
    p10_s = mapnik.PolygonSymbolizer(mapnik.Color('#08306B'))
    


    r1.symbols.append(p1_s)                                             # add the symbolizer to the rule object
    r2.symbols.append(p2_s)
    r3.symbols.append(p3_s)
    r4.symbols.append(p4_s)
    r5.symbols.append(p5_s)
    r6.symbols.append(p6_s)
    r7.symbols.append(p7_s)
    r8.symbols.append(p8_s)
    r9.symbols.append(p9_s)
    r10.symbols.append(p10_s)


    # Filters (ideally computed by Jenk's natural separation)
   
    r1.filter = mapnik.Filter(HH + " < 2000")
    r2.filter = mapnik.Filter(HH + " >= 2000 and " + HH + " < 4000")
    r3.filter = mapnik.Filter(HH + " >= 4000 and " + HH + " < 6000")
    r4.filter = mapnik.Filter(HH + " >= 6000 and " + HH + " < 9000")
    r5.filter = mapnik.Filter(HH + " >= 9000 and " + HH + " < 13000")
    r6.filter = mapnik.Filter(HH + " >= 13000 and " + HH + " < 19000")
    r7.filter = mapnik.Filter(HH + " >= 19000 and " + HH + " < 28000")
    r8.filter = mapnik.Filter(HH + " >= 28000 and " + HH + " < 37000")
    r9.filter = mapnik.Filter(HH + " >= 37000 and " + HH + " < 69000")
    r10.filter = mapnik.Filter(HH + " >= 69000")
    
    l_s = mapnik.LineSymbolizer(mapnik.Color('rgb(50%,50%,50%)'),0.1) # to add outlines to a polygon we create a LineSymbolizer
    r1.symbols.append(l_s)                                              # add the symbolizer to the rule object
    r2.symbols.append(l_s)
    r3.symbols.append(l_s)
    r4.symbols.append(l_s)
    r5.symbols.append(l_s)
    r6.symbols.append(l_s)
    r7.symbols.append(l_s)
    r8.symbols.append(l_s)
    r9.symbols.append(l_s)
    r10.symbols.append(l_s)


    s.rules.append(r1)                                                  # adding rules to the style
    s.rules.append(r2)
    s.rules.append(r3)
    s.rules.append(r4)
    s.rules.append(r5)
    s.rules.append(r6)
    s.rules.append(r7)
    s.rules.append(r8)
    s.rules.append(r9)
    s.rules.append(r10)

    m.append_style('My Style',s)                                        # applying style to the map

    # Data source

    ds = mapnik.Shapefile(file='../output/activity_start.shp')
    
    # Creating a Layer
    # Mapnik Layers are basically containers around datasources, 
    # that store useful properties.
    
    layer = mapnik.Layer('belgium')
    layer.datasource = ds
    layer.styles.append('My Style')

    # Preparing rendering
    
    m.layers.append(layer)
    m.zoom_all()
    
    # Final rendering
 
    f_pdf = open('../output/start_act_'+ HH + '.pdf', 'w')
    surface = cairo.PDFSurface(f_pdf.name, 1200, 600)  
    ctx_pdf = cairo.Context(surface)
  
    # ... rendering 10 images for generating a movie
    for j in range(0,10):

        i = i + 1
                    
        #map rendering
        surface2 = cairo.ImageSurface(cairo.FORMAT_ARGB32, 1200, 600)           
        ctx = cairo.Context(surface2)
        #mapnik.render(m, ctx)
        mapnik.render(m, surface2)

        # title rendering
        ctx.select_font_face('Cantarell', cairo.FONT_WEIGHT_BOLD, cairo.FONT_WEIGHT_NORMAL)
        ctx.set_font_size(24)
        ctx.move_to(30,40)
        ctx.show_text('VirtualBelgium - ' + HH)
        
        # legend
        ctx.set_font_size(19)
        
        ctx.set_source_rgb(217.0/255.0,221.0/255.0,244.0/255.0)
        ctx.rectangle(50,350,17,17)
        ctx.fill()
        ctx.set_source_rgb(0,0,0)
        ctx.move_to(75,350+16)
        ctx.show_text('less than 2000')
            
        ctx.rectangle(50,375,17,17)
        ctx.set_source_rgb(175.0/255.0,185.0/255.0,240.0/255.0)
        ctx.fill()
        ctx.set_source_rgb(0,0,0)
        ctx.move_to(75,375+16)
        ctx.show_text('2000 to 4000')
        
        ctx.rectangle(50,400,17,17)    
        ctx.set_source_rgb(124.0/255.0,141.0/255.0,239.0/255.0)
        ctx.fill()
        ctx.set_source_rgb(0,0,0)
        ctx.move_to(75,400+16)
        ctx.show_text('4000 to 6000')
        
        ctx.rectangle(50,425,17,17)    
        ctx.set_source_rgb(85.0/255.0,108.0/255.0,240.0/255.0)    
        ctx.fill()
        ctx.set_source_rgb(0,0,0)
        ctx.move_to(75,425+16)
        ctx.show_text('6000 to 9000')
        
        ctx.rectangle(50,450,17,17)
        ctx.set_source_rgb(43.0/255.0,72.0/255.0,236.0/255.0)    
        ctx.fill()
        ctx.set_source_rgb(0,0,0)
        ctx.move_to(75,450+16)
        ctx.show_text('9000 to 13000')
            
        ctx.rectangle(50,475,17,17)
        ctx.set_source_rgb(8.0/255.0,42.0/255.0,236.0/255.0)
        ctx.fill()
        ctx.set_source_rgb(0,0,0)
        ctx.move_to(75,475+16)
        ctx.show_text('13000 to 19000')
        
        ctx.rectangle(50,500,17,17) 
        ctx.set_source_rgb(0.0/255.0,19.0/255.0,131.0/255.0)       
        ctx.fill()
        ctx.set_source_rgb(0,0,0)
        ctx.move_to(75,500+16)
        ctx.show_text('19000 to 28000')

        ctx.rectangle(50,525,17,17) 
        ctx.set_source_rgb(0.0/255.0,19.0/255.0,131.0/255.0)       
        ctx.fill()
        ctx.set_source_rgb(0,0,0)
        ctx.move_to(75,525+16)
        ctx.show_text('28000 to 37000')

        ctx.rectangle(50,550,17,17) 
        ctx.set_source_rgb(0.0/255.0,19.0/255.0,131.0/255.0)       
        ctx.fill()
        ctx.set_source_rgb(0,0,0)
        ctx.move_to(75,550+16)
        ctx.show_text('37000 to 69000')

        ctx.rectangle(50,575,17,17) 
        ctx.set_source_rgb(0.0/255.0,19.0/255.0,131.0/255.0)       
        ctx.fill()
        ctx.set_source_rgb(0,0,0)
        ctx.move_to(75,575+16)
        ctx.show_text('more than 69.000')

        
        #end rendering
        

        surface2.write_to_png('../output/' + str(i) + '_start_act.png')
        #surface2.finish()



    ctx_pdf.set_source_surface(surface2)
    ctx_pdf.paint()
    #ctx_pdf.restore()
    surface.finish()

    surface2.write_to_png('../output/start_act_' + HH + '.png')
    
print "Done!"

print "Generating a movie..."

call('ffmpeg -f image2 -framerate 3 -pattern_type sequence -start_number 1 -i ../output/%d_start_act.png -vcodec mpeg4 -pix_fmt yuv420p ../output/activity_start.avi -r 3',shell=True)

print "Cleaning temporary files..."
call("rm ../output/*_start_act.png", shell=True)

print "Done!"
