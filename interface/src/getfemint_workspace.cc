/*===========================================================================

 Copyright (C) 2002-2015 Julien Pommier.

 This file is a part of GetFEM++

 GetFEM++  is  free software;  you  can  redistribute  it  and/or modify it
 under  the  terms  of the  GNU  Lesser General Public License as published
 by  the  Free Software Foundation;  either version 3 of the License,  or
 (at your option) any later version along with the GCC Runtime Library
 Exception either version 3.1 or (at your option) any later version.
 This program  is  distributed  in  the  hope  that it will be useful,  but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or  FITNESS  FOR  A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 License and GCC Runtime Library Exception for more details.
 You  should  have received a copy of the GNU Lesser General Public License
 along  with  this program;  if not, write to the Free Software Foundation,
 Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA.

===========================================================================*/
// $Id$
#define GETFEMINT_WORKSPACE_C

#include <getfem/dal_singleton.h>
#include <getfem/bgeot_config.h>
#include <getfemint_workspace.h>

namespace getfemint
{
  workspace_stack& workspace() {
    return dal::singleton<workspace_stack>::instance();
  }

  void workspace_stack::set_dependance(getfem_object *user, getfem_object *used)
  {
    // if (!used->is_static()) {
      std::vector<id_type> &u = used->used_by;
      if (std::find(u.begin(), u.end(), user->get_id()) == u.end())
        u.push_back(user->get_id());
    // }
  }


  void workspace_stack::sup_dependance(getfem_object *user, getfem_object *used)
  {
    // if (!used->is_static()) {
      std::vector<id_type> &u = used->used_by;
      unsigned i = 0, j = 0;
      for ( ; i < u.size(); ++i) {
        u[j] = u[i];
        if (u[i] != user->get_id()) ++j;
      }
      u.resize(j);
    // }
  }

  /* throw recursively anonymous objects in the zombie workspace */
  void workspace_stack::mark_deletable_objects(id_type id, dal::bit_vector &lst, dal::bit_vector &glst) const {
    if (!obj.index().is_in(id)) THROW_INTERNAL_ERROR;
    getfem_object *o = obj[id];
    if (!o) THROW_INTERNAL_ERROR;
    if (glst.is_in(id) || lst.is_in(id)) return; // already inspected
    if (!o->is_anonymous()) return;
    bool it_is_possible  = true;
    glst.add(id);
    for (unsigned i=0; i < o->used_by.size(); ++i) {
      mark_deletable_objects(o->used_by[i], lst, glst);
      if (!lst.is_in(o->used_by[i])) it_is_possible = false;
    }
    if (it_is_possible) lst.add(id);
  }



  /* this is an experimental function... (there is a small bug in python interface gc).

     unmark the object for future deletion (object becomes from anonymous to current),
     and what else is needed?
   */
  void workspace_stack::undelete_object(id_type id) {
    getfem_object *o = obj[id];
    if (!o) { THROW_ERROR("this object does not exist\n"); }
    if (o->is_static() && o->ikey == 0) { THROW_ERROR("o->is_static() && o->ikey == 0"); }
    if (o->is_anonymous()) {
      o->set_workspace(current_workspace);
      //if (o->ikey) kmap[o->ikey] = o;// would it be necessary??
      //std::cout << "o->ikey       : " << o->ikey << std::endl;
      //std::cout << "o             : " << o << std::endl;
      //std::cout << "kmap[o->ikey] : " << kmap[o->ikey] << std::endl;
      //newly_created_objects.push_back(id); // would it be necessary??
    } // what do you do if o is not anonymous?
  }

  /* at least mark the objet for future deletion (object becomes anonymous)
     and if possible, destroy the object (and all the objects which use this one
     if they are all anonymous) */
  void workspace_stack::delete_object(id_type id) {
    if (obj.index()[id]) {
      //cerr << "delete_object requested: id=" << id << ", type = " << name_of_getfemint_class_id(o->class_id()) << "\n";

      if (!obj[id]) THROW_INTERNAL_ERROR;

      // The next line has been commented because it make the mesh of a level
      // set non deletable (because the mesh_fem of the level_set is static,
      //  never deleted and it depends on the mesh).
      // if (obj[id]->is_static()) return;

      /* mark the object as an anonymous one */
      obj[id]->set_workspace(anonymous_workspace);

      /* list of objects to delete */
      dal::bit_vector dellst;
      for (dal::bv_visitor ii(obj.index()); !ii.finished(); ++ii)
        mark_deletable_objects(id_type(ii), dellst);

      if (dellst.card()) {
        /* clear each deletable objects. After the clear there should
           not__ be anymore dependance between them (i.e. we can
           delete them in any order)
        */
        for (dal::bv_visitor ii(dellst); !ii.finished(); ++ii)
          obj[ii]->clear_before_deletion();

        /* suppress the objects in any order */
        for (dal::bv_visitor ii(dellst); !ii.finished(); ++ii) {
          if (obj[ii]->ikey) kmap.erase(obj[ii]->ikey);
          delete obj[ii];
          obj[ii] = 0;
          obj.sup(ii);
        }

        /* remove the deleted objects from the "used_by" arrays */
        for (dal::bv_visitor ii(obj.index()); !ii.finished(); ++ii) {
          getfem_object *o = obj[ii];
          int j = 0;
          for (unsigned i=0; i < o->used_by.size(); ++i) {
            if (!dellst.is_in(o->used_by[i])) {
              o->used_by[j++] = o->used_by[i];
            }
          }
          o->used_by.resize(j);
        }
      }
    } else {
      std::stringstream s;
      s << "object number " << id << " no longer exists : can't delete it";
      throw getfemint_error(s.str());
    }
  }

  /* inserts a new object (and gives it an id) */
  id_type workspace_stack::push_object(getfem_object *o) {
    id_type obj_id = id_type(obj.add(o));
    //if (!o->is_static())
    o->set_workspace(current_workspace);
    if (o->is_static() && o->ikey == 0)
      THROW_INTERNAL_ERROR;
    o->set_id(obj_id);
    if (o->ikey) kmap[o->ikey] = o;
    //cerr << "kmap[" << o->ikey << "]=" << o << "\n";
    newly_created_objects.push_back(obj_id);
    return obj_id;
  }

  /* create a new workspace on top of the stack */
  void workspace_stack::push_workspace(std::string n) {
    id_type new_workspace
      = id_type(wrk.add(workspace_data(n, current_workspace)));
    current_workspace = new_workspace;
  }

  /* move the object in the parent workspace, in order to prevent
     the object from being deleted when the current workspace will
     be 'poped' */
  void workspace_stack::send_object_to_parent_workspace(id_type obj_id) {
    getfem_object *o = obj[obj_id];
    if (!o) { THROW_ERROR("this object does not exist\n"); }
    if (o->is_anonymous()) THROW_INTERNAL_ERROR;
    if (!wrk.index()[o->get_workspace()]) THROW_INTERNAL_ERROR;
    o->set_workspace(wrk[current_workspace].parent_workspace);
  }

  void workspace_stack::send_all_objects_to_parent_workspace() {
    for (obj_ct::tas_iterator it = obj.tas_begin();
         it != obj.tas_end(); ++it) {
      if ((*it)->get_workspace() == current_workspace) {
        (*it)->set_workspace(wrk[current_workspace].parent_workspace);
      }
    }
  }
  /* delete every object in the workspace, but *does not* delete the workspace itself */
  void workspace_stack::clear_workspace(id_type wid) {
    if (wid == anonymous_workspace) THROW_INTERNAL_ERROR;
    for (dal::bv_visitor_c oid(obj.index()); !oid.finished(); ++oid) {
      if (!obj.index().is_in(oid)) continue;
      id_type owid = obj[oid]->get_workspace();
      if (owid != anonymous_workspace && !wrk.index_valid(owid))
        THROW_INTERNAL_ERROR;
      if (owid == wid) {
        delete_object(id_type(oid));
      }
    }
  }

  /* deletes the current workspace and returns to the parent workspace */
  void workspace_stack::pop_workspace(bool keep_all) {
    if (!wrk.index()[current_workspace]) THROW_INTERNAL_ERROR;
    if (current_workspace == base_workspace) THROW_INTERNAL_ERROR;

    if (keep_all) send_all_objects_to_parent_workspace();
    else clear_workspace();
    id_type tmp = current_workspace;
    current_workspace = wrk[current_workspace].parent_workspace;
    wrk.sup(tmp);
  }

  getfem_object* workspace_stack::object(id_type id, const char *expected_type) {
    getfem_object *o = NULL;
    //cout << "obj.index() == " << obj.index() << ", id= " << id << "\n";
    if (obj.index()[id] &&
        std::find(newly_created_objects.begin(), newly_created_objects.end(),id) == newly_created_objects.end()) {
      o = obj[id];
      if (!o) THROW_INTERNAL_ERROR;
    } else {
      THROW_ERROR("object " << expected_type  << " [id=" << id << "] not found");
    }
    return o;
  }

  getfem_object* workspace_stack::object(getfem_object::internal_key_type k) {
    //cerr << "object(" << k << ")\n";
    if (kmap.find(k) != kmap.end()) return kmap[k];
    else return 0;
  }

  void workspace_stack::commit_newly_created_objects() {
    newly_created_objects.resize(0);
  }

  void workspace_stack::destroy_newly_created_objects() {
    while (newly_created_objects.size()) {
      delete_object(newly_created_objects.back());
      newly_created_objects.pop_back();
    }
  }

}
