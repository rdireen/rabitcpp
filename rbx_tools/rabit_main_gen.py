
import os
import shutil
import filecmp
import argparse
import sys

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
    from packaging import version
except ImportError as e:
    print("Failed to import packaging: " + str(e))
    print("")
    print("You may need to install it using:")
    print("    pip3 install --user packaging")
    print("")
    sys.exit(1)

def get_args():
    parser = argparse.ArgumentParser(
        description='Convert msg files to headers/sources')
    parser.add_argument('-p', dest='template',
                        help='empy template to fill in')
    parser.add_argument('-i', dest='inputdir',
                        help='input directory for main.cpp.em')                   
    parser.add_argument('-o', dest='outputdir',
                        help='output directory for main.cpp')
    parser.add_argument('-m', dest='managers',
                        help='managers to include',
                        nargs="+")
    parser.add_argument('-n', dest='name_space',
                        help='name space to put messages in')
                    
    return parser.parse_args()

class GenerateMain(object):

    def __init__(self, args):
        self.template_file = args.template
        self.template_dir = args.inputdir
        self.managers = args.managers
        self.outdir = args.outputdir
        self.out_name = "main.cpp"

    @classmethod
    def generate(cls, args):
        g = cls(args)
        g.generate_main()

    def generate_main(self):
        print("[rabit]        {} --> {}".format(self.template_file, self.out_name))

        self._touch_temporary_output_director()

        # globals sent to empy for generating file
        em_globals = {
            "managers": self.managers
        }

        self._call_empy_and_create_generated_file(em_globals)

    def _touch_temporary_output_director(self):
         # Make sure output directory exists:
        if not os.path.isdir(self.outdir):
            os.makedirs(self.outdir)

    def _call_empy_and_create_generated_file(self, em_globals):
        template_file = os.path.join(self.template_dir, self.template_file)
        output_file = os.path.join(self.outdir, self.out_name)

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
    

if __name__ == "__main__":
    
    args = get_args()

    print("[rabit] Generating main.cpp")
    GenerateMain.generate(args)

