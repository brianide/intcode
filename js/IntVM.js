const clib = Deno.dlopen("../intcode/intvm.so", {
    vm_create: {
        parameters: [],
        result: "pointer",
    },
    vm_destroy: {
        parameters: ["pointer"],
        result: "void",
    },
    vm_load: {
        parameters: ["pointer", "pointer"],
        result: "void",
    },
    vm_run_til_halt: {
        parameters: ["pointer"],
        result: "void",
    },
    vm_push_input: {
        parameters: ["pointer", "i64"],
        result: "i8",
    },
    vm_output_full: {
        parameters: ["pointer"],
        result: "i8",
    },
    vm_awaiting_input: {
        parameters: ["pointer"],
        result: "i8",
    },
    vm_step: {
        parameters: ["pointer"],
        result: "void",
    },
    vm_run_til_event: {
        parameters: ["pointer", "u8"],
        result: "i8",
    },
    vm_has_output: {
        parameters: ["pointer"],
        result: "i8",
    },
    vm_get_output: {
        parameters: ["pointer"],
        result: "i64",
    },
}).symbols;

const finalizers = new FinalizationRegistry(vmPtr => {
    clib.vm_destroy(vmPtr);
});

export default class IntVM {

    constructor() {
        this.vmPtr = clib.vm_create();
        finalizers.register(this, this.vmPtr);
    }

    loadFile(path) {
        const prog = Deno.readTextFileSync(path).split(/,/g);
        const data = new BigInt64Array([prog.length, ...prog].map(n => BigInt(n)));
        clib.vm_load(this.vmPtr, data);
    }

    run() {
        clib.vm_run_til_halt(this.vmPtr);
    }

    getOutput() {
        const out = [];
        for (let i = clib.vm_has_output(this.vmPtr); i > 0; i--) {
            out.push(clib.vm_get_output(this.vmPtr));
        }
        return out;
    }
}