struct @!uname!@Impl : public ExtmethodImpl {
public:
    void execute(bh_instruction *instr, void* arg) {
        assert(instr->isContiguous());

        // A is our image
        bh_view* A = &instr->operand[1];
        bh_data_malloc(A->base);
        void* A_data = A->base->getDataPtr();

        // B must be a one-dimensional vector with two values
        // thresh and maxval
        bh_view* B = &instr->operand[2];
        bh_data_malloc(B->base);
        void* B_data = B->base->getDataPtr();
        assert(B->base->nelem() == 2);

        // C is our output image
        bh_view* C = &instr->operand[0];
        bh_data_malloc(C->base);
        void* C_data = C->base->getDataPtr();

        switch(A->base->dtype()) {
            @!func!@
            default:
                std::stringstream ss;
                ss << bh_type_text(A->base->dtype()) << " not supported by OpenCV for '@!name!@'.";
                throw std::runtime_error(ss.str());
        } /* end of switch */
    } /* end execute method */
}; /* end of struct */
