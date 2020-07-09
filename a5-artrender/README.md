CSci-4611 Assignment 5: Art Render
# YAHYA ALHINAI #5181597
# Dscriptions of design decisions

-  phong.frag: 
    - half-vector is implement for the shader program

- artsy.frag: 
    - textures where changed to "toonDiffuse.png" and "toonSpecular.png"
    - lookup will be expressed as (0.5(n · l) + 0.5, 0), because texture coordinates only go from 0 to 1.

- outline.vert:
    - The Silhouette is detected if the normal of right normal and left normal are opposing each other
    - Once it is detected the silhouette edge get to be amplified by having increasing its thickness