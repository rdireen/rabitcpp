


import os
import shutil
import filecmp
import argparse
import sys

#=============================================================================
#                     Import 3rd Party
#=============================================================================

try:
    import em
except ImportError as e:
    print("Failed to import em: " + str(e))
    print("")
    print("You may need to install it using:")
    print("    pip3 install --user empy")
    print("")
    sys.exit(1)

try:
    import genmsg.template_tools
except ImportError as e:
    print("Failed to import genmsg: " + str(e))
    print("")
    print("You may need to install it using:")
    print("    pip3 install --user pyros-genmsg")
    print("")
    sys.exit(1)

try:
    from packaging import version
except ImportError as e:
    print("Failed to import packaging: " + str(e))
    print("")
    print("You may need to install it using:")
    print("    pip3 install --user packaging")
    print("")
    sys.exit(1)

#=============================================================================
#                     Parse Arguments from CMake
#=============================================================================

def get_args():
    parser = argparse.ArgumentParser(
        description='Convert msg files to headers/sources')
    parser.add_argument('--headers', help='Generate header files',
                        action='store_true')
    parser.add_argument('--sources', help='Generate source files',
                        action='store_true')
    parser.add_argument('--csharps', help='Generate csharp files',
                        action='store_true')
    parser.add_argument('--python', help='Generate python files',
                        action='store_true')
    parser.add_argument('-d', dest='dir', help='directory with msg files')
    parser.add_argument('-f', dest='file',
                        help="files to convert (use only without -d)",
                        nargs="+")
    parser.add_argument('-i', dest="include_paths",
                        help='Additional Include Paths', nargs="*",
                        default=None)
    parser.add_argument('-e', dest='templatedir',
                        help='directory with template files',)
    parser.add_argument('-o', dest='outputdir',
                        help='output directory for header files')
    parser.add_argument('-t', dest='temporarydir',
                        help='temporary directory')
    parser.add_argument('-p', dest='template',
                        help='empy template to fill in')
    parser.add_argument('-n', dest='name_space',
                        help='name space to put messages in')
                    
    return parser.parse_args()

#=============================================================================
#                     Generate Code
#
# Generates *.h *.cpp and *.cs code from *.msg files
#=============================================================================

class GenerateCodeAbstract(object):

    def __init__(self, args):
        self.template_file = args.template
        self.suffix = None
        self.files = args.file
        self.tempdir = args.temporarydir
        self.templatedir = args.templatedir
        self.name_space = args.name_space
        self.outdir = args.outputdir
        self.msg_context = genmsg.msg_loader.MsgContext.create_default()

    @classmethod
    def generate(cls, args):
        g = cls(args)
        for msg_file in g.files:
            g.generate_one_file(msg_file)
        return g

    def generate_one_file(self, msg_file):
        
        spec = self._parse_msg_file_and_get_genmsg_MsgSpec(msg_file)

        print("[rabit]        {} --> {}".format(msg_file, spec.short_name + self.suffix))

        genmsg.msg_loader.load_depends(self.msg_context, spec, {})

        md5sum = genmsg.gentools.compute_md5(self.msg_context, spec)

        self._touch_temporary_output_director()


        # globals sent to empy for generating file
        em_globals = {
            "file_name_in": msg_file,
            "md5sum": md5sum,
            "msg_context": self.msg_context,
            "spec": spec,
            "nm_space": self.name_space
        }
        self._call_empy_and_create_generated_file(spec, em_globals)

    def copy_to_output(self):
        """
        Copies files from inputdir to outputdir if they don't exist in
        ouputdir or if their content changed
        """
        #print(self.outdir)
        # Make sure output directory exists:
        if not os.path.isdir(self.outdir):
            os.makedirs(self.outdir)

        for input_file in os.listdir(self.tempdir):
            fni = os.path.join(self.tempdir, input_file)
            if os.path.isfile(fni):
                # Check if input_file exists in outpoutdir, copy the file if not
                fno = os.path.join(self.outdir, input_file)
                if not os.path.isfile(fno):
                    shutil.copy(fni, fno)
                    print("[rabit] {0}: new header file".format(fno))  
                    continue

                if os.path.getmtime(fni) > os.path.getmtime(fno):
                    # The file exists in inputdir and outputdir
                    # only copy if contents do not match
                    if not filecmp.cmp(fni, fno):
                        shutil.copy(fni, fno)
                        print("[rabit] {0}: updated".format(input_file))

    def _parse_msg_file_and_get_genmsg_MsgSpec(self, msg_file):
        full_type_name = genmsg.gentools.compute_full_type_name("dt", os.path.basename(msg_file))
        return genmsg.msg_loader.load_msg_from_file(self.msg_context, msg_file, full_type_name)

    def _touch_temporary_output_director(self):
         # Make sure output directory exists:
        if not os.path.isdir(self.tempdir):
            os.makedirs(self.tempdir)

    def _call_empy_and_create_generated_file(self, spec, em_globals):
        template_file = os.path.join(self.templatedir, self.template_file)
        output_file = os.path.join(self.tempdir, spec.short_name + self.suffix)

        folder_name = os.path.dirname(output_file)
        if not os.path.exists(folder_name):
            os.makedirs(folder_name)

        ofile = open(output_file, 'w')
        interpreter = em.Interpreter(output=ofile, globals=em_globals, options={
            em.RAW_OPT: True, em.BUFFERED_OPT: True})
        try:
            interpreter.file(open(template_file))
        except OSError as e:
            ofile.close()
            os.remove(output_file)
            raise
        interpreter.shutdown()
        ofile.close()
    
#=============================================================================
#         Any additional parameters that might be needed go into these 
# overriden classes
#=============================================================================
class GenerateHeaders(GenerateCodeAbstract):
    def __init__(self, args):
        super(GenerateHeaders, self).__init__(args)
        self.suffix = '.h'

class GenerateSources(GenerateCodeAbstract):
    def __init__(self, args):
        super(GenerateSources, self).__init__(args)
        self.suffix = '.cpp'

class GenerateCSharps(GenerateCodeAbstract):
    def __init__(self, args):
        super(GenerateCSharps, self).__init__(args)
        self.suffix = '.cs'

class GeneratePython(GenerateCodeAbstract):
    def __init__(self, args):
        super(GeneratePython, self).__init__(args)
        self.suffix = '.py'

#=============================================================================
#                           MAIN
#=============================================================================

if __name__ == "__main__":
    """ Get the args from CMake and generate code
    """
    
    args = get_args()

    if args.headers:
        print("[rabit] Generating *.h files from *.msg ")
        h = GenerateHeaders.generate(args)
        h.copy_to_output()

    elif args.sources:
        print("[rabit] Generating *.cpp files from *.msg ")
        GenerateSources.generate(args)

    elif args.csharps:
        print("[rabit] Generating *.cs files from *.msg ")
        cs = GenerateCSharps.generate(args)
        cs.copy_to_output()

    elif args.python:
        print("[rabit] Generating *.py files from *.msg ")
        py = GeneratePython.generate(args)
        py.copy_to_output()
        
    else:
        print("[rabit] ERROR: unknowon output type")


















