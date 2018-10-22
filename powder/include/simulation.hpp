#pragma once

enum edge_mode_t
{
	em_void, // Particles that exceed any boundary will be destroyed.
	em_loop, // Particles that exceed any boundary will be warped to the opposite boundary.
	em_edge  // Particles that exceed any boundary will be reflected off the boundary.
};

edge_mode_t edge_mode = em_void;