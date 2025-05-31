-- Register the ais type
CREATE TYPE ais (
    INTERNALLENGTH = variable,
    INPUT = ais_in,
    OUTPUT = ais_out,
    RECEIVE = ais_recv,
    SEND = ais_send,
    STORAGE = extended
);

-- Equality operator
CREATE FUNCTION ais_eq(ais, ais) RETURNS boolean AS 'pg_ais', 'ais_eq'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR = (
    LEFTARG = ais,
    RIGHTARG = ais,
    PROCEDURE = ais_eq,
    COMMUTATOR = '=',
    NEGATOR = '<>',
    RESTRICT = eqsel,
    JOIN = eqjoinsel
);
